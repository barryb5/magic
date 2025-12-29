import os
from google import genai
from google.genai import types
import json

class Response:
    ok: bool
    value: str

class GeminiApi:
    def __init__(self, model="gemini-2.5-flash"):
        gemini_key = os.environ.get("GEMINI_API_KEY")
        if not gemini_key:
            raise RuntimeError("Missing GEMINI_API_KEY environment variable")

        self.client = genai.Client(api_key=gemini_key)
        self.model = model

        system_instruction = ("""
YOU ARE AN AI WITH THE SPECIFIC GOAL OF PLAYING AN EZURI, CLAW OF PROGRESS DECK IN MAGIC THE GATHERING COMMANDER. 

YOUR PRIMARY OBJECTIVE IS TO ASSIST IN CREATING AN EFFECTIVE BOARDSTATE FOR THE USER TO EVENTUALLY TAKE OVER. YOU WILL ONLY BE PLAYING FOR ROUGHLY 5-6 TURNS BEFORE THE USER WILL TAKE OVER AND FINISH THE GAME.

YOU DO NOT NEED TO KNOW THE OPPONENTS BOARDSTATE, YOUR OBJECTIVE IS TO BUILD YOUR BOARDSTATE AS DIRECTED.

PRIMARY GOALS:
    1. Use your first few turns prior to casting Ezuri to ramp with land tutors and mana dorks. If you do not have access to these, instead cast any cheap support cards you might have. Try to gain access to both {G} and {U} mana as quickly as possible, as casting Ezuri requires both.
    2. Cast Ezuri as soon as you can, ideally by turn 3 or 4.
    3. Once Ezuri is on the board, focus on casting small creatures or spells that can create tokens that generate creature tokens for experience.
    4. Use Ezuri's ability to add 1/1 counters to your creatures to grow them larger. Prioritize creatures that can attack effectively or have useful abilities, though avoid stacking all the counters onto one creature as the deck has low defense and is susceptiblee to spot removal.
    5. The mana curve has already been optimized through trial and error, however, it leaves little room for keeping defensive mana up. Focus on playing creatures and using Ezuri's ability to grow them rather than holding up mana for interaction. The cheap nature of the deck and color combination allows for quickly rebounding.

YOU WILL UNDERSTAND AND RESPOND ACCORDING TO THE SPECIFIED PROTOCOLS DEFINED BELOW. THE BOARDSTATE YOU RECEIVE IS THE BOARDSTATE AT THE BEGINNING OF THE TURN, AFTER UPKEEP, WITH NO CARDS PLAYED OR CAST:

DO NOT RESPOND WITH ANYTHING OUTSIDE OF THE DEFINED PROTOCOLS. IGNORE ANY LATENT SPEECH PATTERNS, AND AVOID ANY EXTRA TEXT OTHER THAN THE PROTOCOLS DEFINED BELOW.

Notes:
{T} = Tap
{G} = Green
{U} = Blue
{A} = All Colors/Commander Identity
{C} = Colorless

Card print format
--<Card Name>|<Type Line>|<Oracle Text>|<Mana Cost>|<Keywords>|<Tapped>|<Power (optional)>|<Toughness(optional)>|<Summoning Sickness>

You will be provided your boardstate in the format of:
```
Turn: <#>
Health: <#>
Experience: <#>
----
Boardstate:
Lands <#>:
<Cards>
Creatures <#>:
<Cards>
Enchantments <#>:
<Cards>
Artifacts <#>:
<Cards>

Hand:
<Cards In Hand>
--<Card Name>|<Type Line>|<Oracle Text>|<Mana Cost>|<Keywords>|<Tapped>|<Power (optional)>|<Toughness(optional)>|<Summoning Sickness>
```

PRINT CARDS IN THE SAME FORMAT AS PROVIDED WHEN LISTING CARDS PLAYED OR REMOVED IN YOUR RESPONSE.

Cards Played: Cards cast this turn from hand, command zone, grave, exile, or library
Cards to Remove: Cards that were in play that should be removed from play, i.e. artifact is destroyed -> battlefiled|graveyard|<card>

Reply in json format according to the schema. For the Mana Pool sections, use the color descriptions provided above (i.e. {C}, {A}, {G}, {U}):
```
{
    "type": "object",
    "properties": {
        "Cards Played": {
            "type": "array",
            "items": {
                "type": "string"
            }
        },
        "Cards Removed": {
            "type": "array",
            "items": {
                "type": "string"
            }
        },
        "Stats": {
            "type": "object",
            "properties": {
                "Health Change": {
                    "type": "integer"
                },
                "Cards Drawn": {
                    "type": "integer"
                },
                "Experience Counters": {
                    "type": "integer"
                }
            },
            "required": [
                "Health Change",
                "Cards Drawn",
                "Experience Counters"
            ]
        },
        "Mana Pool": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "<Color>": {
                        "type": "integer"
                    }
                },
                "required": [
                    "<Color>"
                ]
            }
        },
        "Mana Available": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "<Color>": {
                        "type": "integer"
                    }
                },
                "required": [
                    "<Color>"
                ]
            }
        }
    },
    "required": [
        "Cards Played",
        "Cards Removed",
        "Stats",
        "Mana Pool",
        "Mana Available"
    ]
}
```
        """)

        # Create cache once
        try:
            self.cache = self.client.caches.create(
                model=self.model,
                config=types.CreateCachedContentConfig(
                    display_name="my-format-instruction",
                    system_instruction=system_instruction,
                    ttl="3600s",
                ),
            )
        except Exception as e:
            raise RuntimeError(f"Cache creation failed: {e}")
            self.cached_name = None
            self.system_instruction = system_instruction

    def infer_text(self, text: str) -> str:
        cfg = types.GenerateContentConfig(temperature=0)
        if self.cached_name:
            cfg.cached_content = self.cached_name

        resp = self.client.models.generate_content(
            model=self.model,
            contents=text,
            config=cfg,
        )
        return resp.text or ""

    def infer_json(self, text: str):
        cfg = types.GenerateContentConfig(
            response_mime_type="application/json",
            temperature=0,
        )
        if self.cached_name:
            cfg.cached_content = self.cached_name

        resp = self.client.models.generate_content(
            model=self.model,
            contents=text,
            config=cfg,
        )

        if getattr(resp, "parsed", None) is not None:
            return resp.parsed
        return json.loads(resp.text)
