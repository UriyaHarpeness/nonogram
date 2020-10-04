import pygame

with open('solution.txt') as solution_file:
    board = solution_file.read()

board = [list(line) for line in list(filter(None, board.split('\n')))]

x = int(len(board[0]) / 3)
y = len(board)

pygame.init()
done = False
ratio = 31

while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True
        if event.type == pygame.KEYDOWN:
            if event.key in (pygame.K_PLUS, pygame.K_KP_PLUS):
                ratio = min(ratio + 3, 40)
            if event.key in (pygame.K_MINUS, pygame.K_KP_MINUS):
                ratio = max(ratio - 3, 1)

    screen = pygame.display.set_mode((x * ratio, y * ratio))

    for i in range(x):
        for j in range(y):
            pygame.draw.rect(screen,
                             pygame.color.THECOLORS['white'] if (''.join(board[j][i * 3:(i + 1) * 3]) == '   ') else
                             pygame.color.THECOLORS['black'],
                             pygame.Rect(i * ratio, j * ratio, ratio, ratio))

    pygame.display.flip()
