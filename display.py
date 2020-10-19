import argparse
import pygame

INITIAL_R = 31
STEP_R = 3
MIN_R = 1
MAX_R = 40
TILES_GROUPING = 3
BLANK_TILE = ' '


def display(nonogram_path: str):
    with open(nonogram_path) as solution_file:
        board = solution_file.read()

    board = [list(line) for line in list(filter(None, board.split('\n')))]

    x = int(len(board[0]) / TILES_GROUPING)
    y = len(board)

    pygame.init()
    done = False
    ratio = INITIAL_R

    while not done:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True
            if event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_PLUS, pygame.K_KP_PLUS):
                    ratio = min(ratio + STEP_R, MAX_R)
                if event.key in (pygame.K_MINUS, pygame.K_KP_MINUS):
                    ratio = max(ratio - STEP_R, MIN_R)
            if event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 4:
                    ratio = min(ratio + STEP_R, MAX_R)
                if event.button == 5:
                    ratio = max(ratio - STEP_R, MIN_R)

        screen = pygame.display.set_mode((x * ratio, y * ratio))

        for i in range(x):
            for j in range(y):
                pygame.draw.rect(
                    screen,
                    pygame.color.THECOLORS['white'] if (''.join(
                        board[j][i * TILES_GROUPING:(i + 1) * TILES_GROUPING]) == BLANK_TILE * TILES_GROUPING) else
                    pygame.color.THECOLORS['black'],
                    pygame.Rect(i * ratio, j * ratio, ratio, ratio))

        pygame.display.flip()


def main():
    parser = argparse.ArgumentParser(description='Display nonogram result.')

    parser.add_argument('-p', '--path', dest='nonogram_path', required=True, help='Path to nonogram result')

    args = parser.parse_args()

    print(f'Displaying {args.nonogram_path}, use "+", "-", or the mouse scrolls to zoom in and out.')

    display(args.nonogram_path)


if __name__ == '__main__':
    main()
