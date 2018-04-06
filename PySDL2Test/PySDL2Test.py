import os
import sys
from typing import Union, List, Any
from game_world import *

# 添加SDL2.dll 的路径
os.environ.update({"PYSDL2_DLL_PATH": "E:\\Application\\SDL2-2.0.8\\lib\\x64"})
from sdl2.ext import *
from sdl2 import *

RESOURCES = Resources(__file__, "resources")


def drawrect(x, y, w, h, color):
    r: SDL_Rect = SDL_Rect(x, y, w, h)
    fill()
    SDL_FillRect()


def run():
    # 初始化SDL2和相关库
    init()
    # 初始化世界
    world: World = World()
    # 初始化窗口
    window: Window = Window("The Pong Game", size=(800, 600))
    window.show()
    # 初始化精灵工厂
    factory: SpriteFactory = sdl2.ext.SpriteFactory(sdl2.ext.SOFTWARE)

    # 构造精灵组件
    sp_paddle1: Union[TextureSprite, SoftwareSprite] = factory.from_color(RED, size=(20, 100))
    sp_paddle2: Union[TextureSprite, SoftwareSprite] = factory.from_color(WHITE, size=(20, 100))
    sp_ball: Union[TextureSprite, SoftwareSprite] = factory.from_color(WHITE, size=(20, 20))

    # 添加实体到世界
    player1: Player = Player(world, sp_paddle1, 0, 250)
    player2: Player = Player(world, sp_paddle2, 780, 250)
    ball: Ball = Ball(world, sp_ball, 390, 290)

    # 为实体初始化属性
    ball.velocity.vx = 2
    # ball.velocity.vy = -1

    # 构造系统
    spriterenderer: SoftwareRenderer = SoftwareRenderer(window)
    movement: MovementSystem = MovementSystem(0, 0, 800, 600)
    collision = CollisionSystem(0, 0, 800, 600)

    # 添加System
    world.add_system(spriterenderer)
    world.add_system(movement)
    world.add_system(collision)

    # 为系统初始化属性
    collision.ball = ball

    # 消息循环
    running = True
    while running:
        events = sdl2.ext.get_events()
        for event in events:
            if event.type == sdl2.SDL_QUIT:
                running = False
                break
            if event.type == sdl2.SDL_KEYDOWN:
                if event.key.keysym.sym == sdl2.SDLK_UP:
                    player1.velocity.vy = -3
                elif event.key.keysym.sym == sdl2.SDLK_DOWN:
                    player1.velocity.vy = 3
            elif event.type == sdl2.SDL_KEYUP:
                if event.key.keysym.sym in (sdl2.SDLK_UP, sdl2.SDLK_DOWN):
                    player1.velocity.vy = 0
        sdl2.SDL_Delay(10)
        world.process()

    # 退出前清理SDL2
    return quit()


if __name__ == '__main__':
    sys.exit(run())
