from struct import unpack
from typing import List
import pygame as py
import random

class Entity:
    def __init__(self, id) -> None:
        self.x_array : List[float] = []
        self.y_array : List[float] = []
        self.color : py.Color = py.Color(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        self.id : int = id

def main() -> None:
    screen_width : int
    screen_height : int
    entity_size : float
    max_ticks : int
    epoch_number : int
    game_array : List[List[Entity]] = []
    target_point_x_array : List[float] = []
    target_point_y_array : List[float] = []
    with open("data.bin", "rb") as file:
        screen_width = int(unpack("f", file.read(4))[0])
        screen_height = int(unpack("f", file.read(4))[0])
        entity_num = unpack("i", file.read(4))[0]
        entity_size = unpack("f", file.read(4))[0]
        max_ticks = unpack("i", file.read(4))[0]
        epoch_number = unpack("i", file.read(4))[0]
        for epoch in range(epoch_number):
            entity_array : List[Entity] = [Entity(id) for id in range(entity_num)]
            target_point_x_array.append(unpack("f", file.read(4))[0])
            target_point_y_array.append(unpack("f", file.read(4))[0])
            for tick in range(max_ticks):
                for ent in range(entity_num):
                    entity_array[ent].x_array.append(unpack("f", file.read(4))[0])
                    entity_array[ent].y_array.append(unpack("f", file.read(4))[0])
            game_array.append(entity_array)

    while True:
        input_val = input(f"There are {epoch_number} games - which one would you like to display? (\"Exit\" to exit)\n")
        if input_val == "Exit":
            break
        game_index : int = int(input_val) - 1
        print(f"target point: {target_point_x_array[game_index]} - {target_point_y_array[game_index]}")
        tick : int = 0
        py.init()
        screen = py.display.set_mode((screen_width, screen_height))
        clock = py.time.Clock()
        running : bool = True
        while running:
            for event in py.event.get():
                if event.type == py.QUIT:
                    running = False
            screen.fill("white")
            py.draw.circle(screen, py.Color(200, 200, 200), py.Vector2(target_point_x_array[game_index], target_point_y_array[game_index]), 100)
            for ent in game_array[game_index]:
                py.draw.circle(screen, ent.color, py.Vector2(ent.x_array[tick], ent.y_array[tick]), entity_size)
            py.display.flip()
            tick += 1
            if tick == max_ticks:
                running = False
            clock.tick(60)
        py.quit()

if __name__ == "__main__":
    main()