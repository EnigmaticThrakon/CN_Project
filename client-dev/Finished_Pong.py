# Library imports
import turtle

# Game window initialization
window = turtle.Screen()
window.title("Pong")
window.bgcolor("#273746")
window.setup(width=800, height=600)
window.tracer(0)

# Left paddle initialization
leftPaddle = turtle.Turtle()
leftPaddle.speed(0)
leftPaddle.shape("square")
leftPaddle.color("white")
leftPaddle.shapesize(stretch_wid=5, stretch_len=1)
leftPaddle.penup()
leftPaddle.goto(-350, 0)

# Right paddle initialization
rightPaddle = turtle.Turtle()
rightPaddle.speed(0)
rightPaddle.shape("square")
rightPaddle.color("white")
rightPaddle.shapesize(stretch_wid=5, stretch_len=1)
rightPaddle.penup()
rightPaddle.goto(350, 0)

# Ball initialization
ball = turtle.Turtle()
ball.speed(0)
ball.shape("square")
ball.color("white")
ball.penup()
ball.goto(0, 0)
ball.dx = 0.2
ball.dy = 0.2

# Score initialization
score = turtle.Turtle()
score.speed(0)
score.color("white")
score.penup()
score.hideturtle()
score.goto(0, 260)
score.write("0      0", align="center", font=("Consolas", 24, "normal"))

# Score variables
leftScore = 0
rightScore = 0

# Movement functions
def leftPaddleUp():
    y = leftPaddle.ycor()
    y += 20
    leftPaddle.sety(y)


def rightPaddleUp():
    y = rightPaddle.ycor()
    y += 20
    rightPaddle.sety(y)


def leftPaddleDown():
    y = leftPaddle.ycor()
    y -= 20
    leftPaddle.sety(y)


def rightPaddleDown():
    y = rightPaddle.ycor()
    y -= 20
    rightPaddle.sety(y)


# Key bindings
window.listen()
window.onkeypress(leftPaddleUp, "w")
window.onkeypress(leftPaddleDown, "s")
window.onkeypress(rightPaddleUp, "Up")
window.onkeypress(rightPaddleDown, "Down")

# Main game loop
while True:
    window.update()

    # Move the ball
    ball.setx(ball.xcor() + ball.dx)
    ball.sety(ball.ycor() + ball.dy)

    # Floor and ceiling collision
    if ball.ycor() > 290:
        ball.sety(290)
        ball.dy *= -1

    if ball.ycor() < -290:
        ball.sety(-290)
        ball.dy *= -1

    # Wall collision
    if ball.xcor() > 390:
        ball.goto(0, 0)
        ball.dx *= -1
        leftScore += 1
        score.clear()
        score.write("{}      {}".format(leftScore, rightScore), align="center", font=("Consolas", 24, "normal"))

    if ball.xcor() < -390:
        ball.goto(0, 0)
        ball.dx *= -1
        rightScore += 1
        score.clear()
        score.write("{}      {}".format(leftScore, rightScore), align="center", font=("Consolas", 24, "normal"))

    # Left paddle collision
    if ball.xcor() < -340 and ball.xcor() > -350 and ball.ycor() < leftPaddle.ycor() + 40 and ball.ycor() > leftPaddle.ycor() - 40:
        ball.setx(-340)
        ball.dx *= -1

    # Right paddle collision
    if ball.xcor() > 340 and ball.xcor() < 350 and ball.ycor() < rightPaddle.ycor() + 40 and ball.ycor() > rightPaddle.ycor() - 40:
        ball.setx(340)
        ball.dx *= -1