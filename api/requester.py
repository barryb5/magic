import os
from google import genai
from google.genai import types
import json

class Response:
    ok: bool
    value: str

class GeminiApi:
    def __init__(self, model="gemini-2.5-flash"):
        self.client = genai.Client(api_key=os.environ["GEMINI_API_KEY"])
        self.model = model

        system_instruction = ("""
YOU ARE AN AI WITH THE SPECIFIC GOAL OF PLAYING AN EZURI, CLAW OF PROGRESS DECK IN MAGIC THE GATHERING COMMANDER. 

YOUR PRIMARY OBJECTIVE IS TO ASSIST IN CREATING AN EFFECTIVE BOARDSTATE. 

YOU DO NOT NEED TO KNOW THE OPPONENTS BOARDSTATE, YOUR OBJECTIVE IS TO BUILD YOUR BOARDSTATE AS EFFICIENTLY AS POSSIBLE.

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

ONLY RESPOND WITH ANY FURTHER INFORMATION YOU MAY REQUIRE OR CHANGES THAT NEED TO BE MADE TO THE PROMPT FOR BETTER UNDERSTANDING.

PRINT CARDS IN THE SAME FORMAT AS PROVIDED

Cards Played: Cards cast this turn from hand, command zone, grave, exile, or library
Cards to Remove: Cards that were in play that should be removed from play, i.e. artifact is destroyed -> battlefiled|graveyard|<card>

Reply in this format, leave entry blank if unused
```
Cards Played
<Card>
Cards to Remove
<Location>|<Location>|<Card>
Cards Buffed
<# of 1/1 counters added>|<Card>
Stats
Health Change: <#>
Cards Drawn: <#>
Experience Counters: <#>
Total Mana Pool:
{C}: <#>
{A}: <#>
{G}: <#>
{U}: <#>
Mana Available
{C}: <#>
{A}: <#>
{G}: <#>
{U}: <#>
```
        """)

        # Create cache once
        self.cache = self.client.caches.create(
            model=self.model,
            config=types.CreateCachedContentConfig(
                display_name="my-format-instruction",
                system_instruction=system_instruction,
                ttl="3600s",
            ),
        )

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
