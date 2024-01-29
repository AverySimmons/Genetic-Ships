
# Genetic Ships

---

**Tools Used:** C, Python, Pygame &nbsp;&nbsp;&nbsp;&nbsp; **Keywords:** Genetic learning, Optimization, Collision, Low level

---

### Description:
&nbsp;&nbsp;&nbsp;&nbsp;A genetic learning project where a population of ray casting ships must fly to a point without colliding. Each ship has 5 short rays that it casts in front of it, then it takes the lengths of those rays along with the local position of the target point and uses them as inputs for its neural network. The neural network produces two outputs, one controlling the rotation speed and direction of the ship and the other controlling its forward velocity. After the models are trained, the data is written in binary to a file, from which it can be parsed using a custom function into a Python program that displays the game. All the game, neural network, and genetic learning logic of this project were built from the ground up in C.


### Features:
- &nbsp;&nbsp;&nbsp;&nbsp;**Low Level:**  
The majority of logic in this project is built from scratch in C, thus all memory allocation is done manually. This includes all the data structures needed to create neural networks, game objects, and helper structures like linked lists, grids, and vectors. In addition this project features custom reading and writing functions that use binary to store and read data from the data.bin file.

- &nbsp;&nbsp;&nbsp;&nbsp;**Game Optimization:**  
Each ship must undergo several collision checks. Firstly, its five ray casts need to assess collision against both the edges of the screen and other ships, involving line-to-line and line-to-circle collision checks. Secondly, each ship must verify whether it is within the target area, necessitating circle-to-circle collision checks. While determining whether each ship is in the target region is sufficiently fast, the challenge lies in the fact that each ray cast must assess potential collisions with every other ship, resulting in an O(n^2) time complexity. To mitigate this inefficiency, I have devised a spatial partitioning system in which each ship is categorized into a spatial grid. Subsequently, each ray only examines its own grid and adjacent grid cells, leading to a time complexity that, in most scenarios, behaves as a more favorably O(n).

- &nbsp;&nbsp;&nbsp;&nbsp;**Machine Learning:**  
The training of the model in this project is done through a custom genetic learning algorithm. The algorithm first generates a random population then begins to loop through the steps of evaluation, crossover, and mutation. In the evaluation step, the game is run with the current population, and each ship is awarded fitness for staying in the target zone, and loses fitness for colliding with other ships and flying off-screen. Next, in the crossover step, the top half of the population carries over to the next population, and the other half is filled with random mixes of their parameters. Finally, random parameters in each model are slightly changed, effectively mutating each network.


### Code Breakdown:
- &nbsp;&nbsp;&nbsp;&nbsp;**Game:**  
Contains all game code, including ship and game structures as well as collision logic.

- &nbsp;&nbsp;&nbsp;&nbsp;**Brain:**  
Contains all the logic and data structures needed to create and compute neural networks as well as all genetic learning code.

- &nbsp;&nbsp;&nbsp;&nbsp;**Display:**  
The python code responsible for reading and displaying replay data.

- &nbsp;&nbsp;&nbsp;&nbsp;**Main:**  
File containing the primary code for starting the learning process.
