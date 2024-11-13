from fastapi import FastAPI, WebSocket, Request
from fastapi.responses import HTMLResponse, FileResponse
import uvicorn

import random
import string
from collections import defaultdict

app = FastAPI()

# Initialize app state to store the compiled string of messages
app.state.received = ""
app.state.solved = [False, False, False, False, False, False]
app.state.explosions = 0
app.state.invalid = False


@app.get("/", response_class=HTMLResponse)
async def get(request: Request):
    # Generate HTML content for stages based on the `solved` list
    stages_html = "".join(
        f"<p><strong>Stage {i + 1}:</strong> <span style='color: {'green' if status else '#555'};'>{'Passed' if status else 'Not Passed'}</span></p>"
        for i, status in enumerate(app.state.solved)
    )

    # Conditionally show the invalid message if `invalid` is True
    invalid_message = "<p style='color: red;'>Invalid input detected!</p>" if app.state.invalid else ""

    # Inline style logic for explosion count color
    explosions_style = "color: red; font-weight: bold;" if app.state.explosions > 0 else "color: #555; font-weight: bold;"

    html_content = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Megan's Bomb 💣</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #f0f2f5;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
            }}
            .container {{
                text-align: center;
                background-color: #fff;
                padding: 2em;
                border-radius: 10px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            }}
            h1 {{
                color: #333;
            }}
            .status-card {{
                background: #f9f9f9;
                padding: 1em;
                border-radius: 5px;
                margin-top: 1em;
                box-shadow: inset 0 0 5px rgba(0, 0, 0, 0.1);
            }}
            .status-card p {{
                font-size: 1.2em;
                color: #555;
            }}
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Megan's Bomb 💣</h1>
            <p><i>Muahahahaha!</i></p>
            <a href="http://108.49.20.247/bomb">Download Bomb</a>
            <div class="status-card">
                {stages_html}
                <p style="{explosions_style}"><strong>Explosions:</strong> {app.state.explosions}</p>
                {('💥' * app.state.explosions) + "💣"}
                {invalid_message}
            </div>
        </div>
    </body>
    </html>
    """
    return HTMLResponse(content=html_content)


@app.get("/bomb")
async def download_bomb():
    # Replace 'path/to/bomb_executable.py' with the actual path to your executable
    file_path = "bomb"
    return FileResponse(file_path, media_type='application/octet-stream', filename="bomb")


def char_map_indexes(s):
    char_map = defaultdict(list)
    for idx, char in enumerate(s):
        char_map[char] = idx
    return dict(char_map)

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    
    await websocket.accept()
    
    seed = ''.join(random.sample(string.ascii_letters + string.digits, 7))
    codes = char_map_indexes(seed)
    
    try:

        await websocket.send_text(seed)
        
        while True:
            data = await websocket.receive_text()
            app.state.received += f"{data}\n"

            if data not in codes:
                app.state.invalid = True
            elif codes[data] == 0: 
                app.state.explosions += 1
            else:
                app.state.solved[codes[data]-1] = True
            

    except Exception as e:
        print(f"WebSocket connection closed: {e}")

if __name__ == "__main__":
    # Run server on port 80
    uvicorn.run(app, host="0.0.0.0", port=80)
