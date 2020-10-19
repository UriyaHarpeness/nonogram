# Nonogram

Welcome to Nonogram!

Got a [nonogram](https://en.wikipedia.org/wiki/Nonogram) on your mind that just doesn't go away, but it takes you too
long to solve? Can't stand looking anymore at a pencil and a piece of paper? Want to cheat at some friendly competition?

Nonogram got your back, simply describe the nonogram in a formatted text file and let Nonogram do the rest!

## Getting Started

First off clone this project, after that you'll need to install a couple of requirements, and than you'll only need to
build it and enjoy the ride.

### Prerequisites

The packages required for building the project are:
gcc, cmake.

```shell script
apt install gcc cmake
```

For using the Python display code, pygame is also required.

```shell script
pip3 install pygame
```

### Configuring

* Logging can be configured [here](logging/debug.h).

### Building

After installing all the requirements, building the project is as straight-forward as it can be.

```
cmake .
make
```

That's it!

### Usage

Nonogram takes 3 keyword arguments, 1 required and 2 optional: `--input`, `--log-level`, and `--output`.
* `--input` is required, the file path containing the nonogram definition.
* `--log-level` is optional, the log level to use, can be one of the following options:
  * ERROR
  * WARNING
  * INFO (default value)
  * DEBUG
  * TRACE
* `--output` is optional, the file path to write the final result to, defaults to `solution.txt`.

The format for input files is as follows:
* All numbers of the same line are split with a space.
* First line is the width and height of the nonogram.
* Next lines describe the nonogram's numbers, vertical first (top to bottom), and then horizontal (left to right).
* An empty lines means no numbers.
* A line that starts with `*` will be ignored.

Refer to the [resources](resources) folder for examples. 

For example:

```text
3 2             // Width of 3 and height of 2.
* Vertical      // Comment, ignored.
2               // 2
                // Empty line.
* Horizontal    // Comment, ignored.
1               // 1
1               // 1
1 1             // 1 1
```

Example for optional result of the file:

```text
[2038/10/28 01:14:28.92] ERROR:   Board size 3 X 2
[2038/10/28 01:14:28.92] ERROR:   
[2038/10/28 01:14:28.92] ERROR:   Solution
[2038/10/28 01:14:28.92] ERROR:   @@@      
[2038/10/28 01:14:28.92] ERROR:   @@@   @@@
```

The python display script takes one keyword argument: `--path` - the path to the nonogram solution file, for example:

```shell script
python3 display.py --path solution.txt
```

You can use the `+`, `-`, and the mouse scroll to zoom in or out on the nonogram.

### Test Run

Now try using Nonogram, take it for a test run, for example:

```text
>>> ./nonogram --name resources/uh.txt --log-level ERROR --output uh-solution.txt

[2038/06/25 11:28:04.31] ERROR:   Board size 25 X 20
[2038/06/25 11:28:04.32] ERROR:   Solution
[2038/06/25 11:28:04.32] ERROR:   @@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@
[2038/06/25 11:28:04.32] ERROR:   @@@         @@@         @@@         @@@         @@@         @@@         @@@
[2038/06/25 11:28:04.32] ERROR:                                                                              
[2038/06/25 11:28:04.32] ERROR:      @@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@   
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@@@@@@@@@@@@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:            @@@@@@         @@@@@@            @@@@@@@@@         @@@@@@@@@      
[2038/06/25 11:28:04.32] ERROR:               @@@@@@@@@@@@@@@            @@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@   
[2038/06/25 11:28:04.32] ERROR:                                                                              
[2038/06/25 11:28:04.32] ERROR:   @@@         @@@         @@@         @@@         @@@         @@@         @@@
[2038/06/25 11:28:04.32] ERROR:   @@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@@@@   @@@@@@
```

And using the display:

```text
python3 display.py --path uh-solution.txt
```

![display.py screen for uh-solution.txt](display-example.png)

## Technologies and Capabilities

* Nonogram is written in [C++](https://en.wikipedia.org/wiki/CPP) (and 
[Python](https://en.wikipedia.org/wiki/Python_(programming_language)) 3 for the display).
* Resolves lines by their possible number of combinations, adding one only after no more progress can be made, giving
the most efficient usage of lines.
* Filters possibilities as soon as they generate before saving them, to reduce memory usage.
* Implements high levels of optimizations, reducing options generated to minimum, causing a much faster run.
* Supports multiple levels of logging.
* Saves result to a file.
* Implements a python code which uses [Pygame](https://www.pygame.org/) to represent the nonogram's solution.

## Documentation

The code is thoroughly documented, using
[LSST DM Developer Guide](https://developer.lsst.io/cpp/api-docs.html#documenting-c-code) format.

I hope that it can answer whichever questions that may arise.

## Contributing

Feel free to contact me if you have any comments, questions, ideas, and anything else you think I should be aware of.
Also, tell me what beautiful and hard nonograms you solved using Nonogram, I'd love to know.

## Authors

* [**Uriya Harpeness**](https://github.com/UriyaHarpeness)

## Acknowledgments

* I would like to thank my wife - Tohar Harpeness, my parents, my computer, and my free time, for enabling me to work on
this small project, it has been fun.

* Big thanks to [Valgrind](https://valgrind.org/) for their dynamic analysis tool, which helped me solve segmentation
faults when trying to add features and optimizations.
