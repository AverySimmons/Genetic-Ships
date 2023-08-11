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
    entity_array : List[Entity]
    entity_size : float
    max_ticks : int
    with open("data.bin", "rb") as file:
        screen_width = int(unpack("f", file.read(4))[0])
        screen_height = int(unpack("f", file.read(4))[0])
        entity_num = unpack("i", file.read(4))[0]
        entity_size = unpack("f", file.read(4))[0]
        max_ticks = unpack("i", file.read(4))[0]
        entity_array = [Entity(id) for id in range(entity_num)]
        current_index : int = 0
        while True:
            x_bytes = file.read(4)
            if not x_bytes:
                break
            x_value : float = unpack("f", x_bytes)[0]
            y_value : float = unpack("f", file.read(4))[0]
            entity_array[current_index].x_array.append(x_value)
            entity_array[current_index].y_array.append(y_value)
            current_index += 1
            if current_index == entity_num:
                current_index = 0
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
        py.draw.circle(screen, py.Color(200, 200, 200), py.Vector2(213, 244), 100)
        for ent in entity_array:
            py.draw.circle(screen, ent.color, py.Vector2(ent.x_array[tick], ent.y_array[tick]), entity_size)
        py.display.flip()
        tick += 1
        if tick == max_ticks:
            running = False
        clock.tick(60)

if __name__ == "__main__":
    main()