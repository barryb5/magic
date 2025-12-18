import json
import time
import sys
from pathlib import Path
import requests

SCRYFALL_API = "https://api.scryfall.com"
HEADERS = {
    # Required by Scryfall
    "User-Agent": "MTGTextFetcher/1.0 (contact: you@example.com)",
    "Accept": "application/json;q=0.9,*/*;q=0.8",
}

def get_card_named_session(
    session: requests.Session,
    name: str,
    *,
    exact: bool = False,
    set_code: str | None = None,
    polite_delay_s: float = 0.075,
    max_retries: int = 5,
) -> dict:
    """
    Fetch a single card object using /cards/named.
    - exact=True uses ?exact=
    - exact=False uses ?fuzzy=
    - set_code limits the result to a specific set printing
    """
    params = {"exact" if exact else "fuzzy": name}
    if set_code:
        params["set"] = set_code

    backoff = 0.25
    for attempt in range(max_retries):
        time.sleep(polite_delay_s)

        r = session.get(f"{SCRYFALL_API}/cards/named", headers=HEADERS, params=params, timeout=15)

        if r.status_code == 404:
            raise LookupError(r.json().get("details", f"Card not found / ambiguous: {name}"))

        if r.status_code == 429 or 500 <= r.status_code < 600:
            # Retry with backoff; respect Retry-After if provided
            retry_after = r.headers.get("Retry-After")
            if retry_after:
                try:
                    time.sleep(float(retry_after))
                except ValueError:
                    time.sleep(backoff)
            else:
                time.sleep(backoff)
            backoff = min(backoff * 2, 5.0)
            continue

        r.raise_for_status()
        return r.json()

    raise RuntimeError(f"Failed to fetch '{name}' after {max_retries} retries (last status {r.status_code}).")


class Card:
    name: str
    id: str
    mana_cost: str
    type_line: str
    oracle_text: str
    power: int | None
    toughness: int | None
    keywords: list[str]
    all_parts: list[dict] | None

def organize_card_data(card_data: dict) -> Card:
    card = Card()
    card.name = card_data.get("name", "")
    card.id = card_data.get("id", "")
    card.mana_cost = card_data.get("mana_cost", "")
    card.type_line = card_data.get("type_line", "")
    card.oracle_text = card_data.get("oracle_text", "")
    card.power = int(card_data["power"]) if "power" in card_data and card_data["power"].isdigit() else None
    card.toughness = int(card_data["toughness"]) if "toughness" in card_data and card_data["toughness"].isdigit() else None
    card.keywords = card_data.get("keywords", [])
    card.all_parts = card_data.get("all_parts", [])
    return card


def parse_deck_file(deck_path: Path) -> dict[str, int]:
    """
    Parses lines like:
      1 Arcane Denial
      18 Forest
    into { "Arcane Denial": 1, "Forest": 18, ... }
    """
    counts: dict[str, int] = {}
    for raw in deck_path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue

        # Split on first space "<count> <name>"
        parts = line.split(" ", 1)
        if len(parts) != 2:
            continue

        qty_str, name = parts[0].strip(), parts[1].strip()
        try:
            qty = int(qty_str)
        except ValueError:
            continue

        counts[name] = counts.get(name, 0) + qty
    return counts

def build_card_library(deck_counts: dict[str, int]) -> list[dict]:
    """
    Returns a list of JSON-serializable dicts like:
      { "count": 2, "card": { ...organized fields... } }
    """
    library: list[dict] = []
    failures: list[dict] = []

    with requests.Session() as session:
        for name, qty in deck_counts.items():
            try:
                print("Fetching:", name)
                raw = get_card_named_session(session, name, exact=False)
                organized = organize_card_data(raw)

                library.append({
                    "count": qty,
                    "card": organized.__dict__,  # JSON-ready
                })
            except Exception as e:
                failures.append({"name": name, "count": qty, "error": str(e)})

    # Optional: include failures at the end so you can see what didn't resolve
    if failures:
        library.append({"_failures": failures})

    return library

if __name__ == "__main__":
    root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path.cwd().resolve()
    deck_path = root / "cards_raw" / "deck.txt"
    out_path  = root / "cards_raw" / "card_library.txt"

    deck_counts = parse_deck_file(deck_path)
    library = build_card_library(deck_counts)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(json.dumps(library, ensure_ascii=False, indent=2), encoding="utf-8")
