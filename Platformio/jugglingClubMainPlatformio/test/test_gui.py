import tkinter as tk
from tkinter import ttk
import os

def run_command1():
    # netsh wlan connect name="Ian's Juggling Clubss"
    os.system("netsh wlan connect name=\"Ian's Juggling Clubss\"")

def run_command2():
    # netsh wlan connect name="The Grand Duchy"
    os.system("netsh wlan connect name=\"The Grand Duchy\"")

def run_command3():
    ip_address = combo.get()
    # curl -X POST -i http://{192.168.4.1}/submit -d "@C:/dev/juggling-clubs/Platformio/jugglingClubMainPlatformio/.pio/libdeps/juggling_club_main/sample_program.json" -H "Content-Type: application/json"
    os.system(f'curl -X POST -i http://{ip_address}/submit -d "@C:\\dev\\juggling-clubs\\Platformio\\jugglingClubMainPlatformio\\.pio\\libdeps\\juggling_club_main\\sample_program.json" -H "Content-Type: application/json"')
    if ip_address not in combo['values']:
        combo['values'] = (ip_address, *combo['values'])


root = tk.Tk()
root.title("Test Device")
root.geometry("240x150")


button1 = tk.Button(root, text="Connect to device", command=run_command1)
button1.pack(padx=10, pady=5)


button3 = tk.Button(root, text="Send JSON to device", command=run_command3)
button3.pack(padx=10, pady=5)

button2 = tk.Button(root, text="Connect to Grand Duchy", command=run_command2)
button2.pack(padx=10, pady=5)

combo = ttk.Combobox(root)
combo.pack(padx=10, pady=5)


root.mainloop()