#include the Flask, request, and jsonify modules from the flask library
from flask import Flask, request, jsonify
import subprocess
from consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY
import re

app=Flask(__name__)


exe_path = r"D:/UE54Source/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe"
uproject = r"D:/GASTPS/GASTPS.uproject"

# TODO: Remove when using docker in the future
NextAvailablePort = 7777

def CreateServerLocalTest(SessionName, SessionSearchId):
    global NextAvailablePort
    subprocess.Popen([
        exe_path,
        uproject,
        "-server",
        "-log",
        '-epicapp="ServerClient"',
        f'-SESSION_NAME="{SessionName}"',
        f'-SESSION_SEARCH_ID="{SessionSearchId}"',
        f'-PORT={NextAvailablePort}'
    ])

    UsedPort = NextAvailablePort
    NextAvailablePort += 1
    return UsedPort


@app.route('/Sessions', methods=['POST'])
def CreateServer():
    print(dict(request.headers))

    SessionName = request.get_json().get(SESSION_NAME_KEY)
    SessionSearchId = request.get_json().get(SESSION_SEARCH_ID_KEY)

    port = CreateServerLocalTest(SessionName, SessionSearchId)
    return jsonify({"status": "success", PORT_KEY: port}), 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=99)
