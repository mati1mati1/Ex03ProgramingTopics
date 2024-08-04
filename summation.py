import os

def parse_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        num_steps = 0
        dirt_left = 0
        status_dead = False

        for line in lines:
            if line.startswith("NumSteps"):
                num_steps = int(line.split('=')[1].strip())
            elif line.startswith("DirtLeft"):
                dirt_left = int(line.split('=')[1].strip())
            elif line.startswith("Status") and "DEAD" in line:
                status_dead = True
        
        return num_steps, dirt_left, status_dead

def sum_values(directory):
    total_steps = 0
    total_dirt_left = 0
    dead_status_found = False

    for filename in os.listdir(directory):
        if filename.endswith(".txt"):  # assuming the files have .txt extension
            file_path = os.path.join(directory, filename)
            num_steps, dirt_left, status_dead = parse_file(file_path)
            total_steps += num_steps
            total_dirt_left += dirt_left
            if status_dead:
                dead_status_found = True

    return total_steps, total_dirt_left, dead_status_found

directory_path = 'test/examples/gt'  
total_steps, total_dirt_left, dead_status_found = sum_values(directory_path)

print("Total NumSteps:", total_steps)
print("Total DirtLeft:", total_dirt_left)
if dead_status_found:
    print("Notification: One or more files have Status == DEAD")