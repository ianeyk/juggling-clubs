import os
import re
import time
ip_address = "10.29.135.1"

# netsh wlan connect name="Ian's Juggling Clubss"
os.system("netsh wlan connect name=\"Ian's Juggling Clubss\"")
time.sleep(5) # needed so the connection can be established

with open('test/send_json_testbench.json') as f:
    lines = "".join([line for line in f]).strip()
    jsons = re.split(r"///+", lines) # three or more slashes

    for json in jsons:
        json_stripped = json.strip().replace('\n', '').replace('"', '\\"')
        curl_command = f'curl -X POST -i http://{ip_address}/submit -d "{json_stripped}"  -H "Content-Type: application/json"'
        print("sending curl command: ")
        print(curl_command)
        print()
        os.system(curl_command)

        time.sleep(1)

# netsh wlan connect name="The Grand Duchy"
os.system("netsh wlan connect name=\"The Grand Duchy\"")

print("finished!")