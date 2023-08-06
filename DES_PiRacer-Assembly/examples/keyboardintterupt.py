import keyboard

def print_press_keys(e):
	print(f"Pressed key: {e.name}")
	
keyboard.on_press(print_press_keys)

print("Press any key (press 'q' to quit)")
while True:
	if keyboard.is_pressed('q'):
		break
		
