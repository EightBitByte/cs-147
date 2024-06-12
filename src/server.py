from flask import Flask, request
import json
import matplotlib.pyplot as plt
import os
import numpy as np

app = Flask(__name__)
json_file_path = "./inbound_info/data.json"
graph_file_path = "./outbound_info/graph.png"

count = 0
user_dict: dict = {}

def update_json(username: str, is_success: int):
    """
    Updates user_dict with the correct and failed attempts.
    """

    with open(json_file_path, 'r') as read_file:
        user_dict = json.load(read_file)

    # If already exists
    if username in user_dict.keys():
        if is_success:
            user_dict[username]["successfulAttempts"] = str(int(user_dict[username]["successfulAttempts"]) + 1)
        else:
            user_dict[username]["failedAttempts"] = str(int(user_dict[username]["failedAttempts"]) + 1)
    # If new entry
    else:
        if is_success:
            user_dict.update({username: {"successfulAttempts": '1', "failedAttempts": '0'}})
        else:
            user_dict.update({username: {"successfulAttempts": '0', "failedAttempts": '1'}})


    with open(json_file_path, 'w') as write_file:
        json.dump(user_dict, write_file)
    

@app.route('/', methods=['GET'])
def index():
    username = request.args.get("userName")
    is_success = request.args.get("isSuccess")

    update_json(username, int(is_success))

    return f'Received {username}\'s data'

@app.route("/graph", methods=['GET'])
def graphing():
    create_plot()
    return f'Created plot'

if __name__ == '__main__':
    app.run()

def create_plot():
    """
    Creates a new correct/incorrect attempts plot and stores the plot as a 
    file in ./outbound_data/graph.png
    """
    plt.style.use('_mpl-gallery')

    with open(json_file_path, 'r') as read_file:
        user_dict = json.load(read_file)

    width = 0.4

    x = np.arange(len(user_dict.keys()))
    userNames = list(user_dict.keys())
    successfulAttempts = [int(user_dict[user]["successfulAttempts"]) for user in userNames]
    unsuccessfulAttempts = [int(user_dict[user]["failedAttempts"]) for user in userNames]

    print(f'userNames: {userNames}')
    print(f"successfulAttempts: {successfulAttempts}")
    print(f"unsuccessfulAttempts: {unsuccessfulAttempts}")

    fig, ax = plt.subplots()
    bars1 = ax.bar(x, successfulAttempts, width, label='Successful', color='b')
    bars2 = ax.bar(x, unsuccessfulAttempts, width, bottom=successfulAttempts, label='Unsuccessful', color='r')

    ax.set_xlabel('Users')
    ax.set_ylabel('Number of Login Attempts')
    ax.set_title('Login Attempts by Users')
    ax.set_xticks(x)
    ax.set_xticklabels(userNames)
    ax.legend()

    max_attempts = max(max(successfulAttempts), max(unsuccessfulAttempts))
    ax.set_ylim(0, max_attempts + 10)  # Add some padding to the top

    plt.savefig(graph_file_path)