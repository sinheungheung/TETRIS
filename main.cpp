#include <SFML/Graphics.hpp>
#include <time.h>
#include <SFML/Audio.hpp> 
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
// API에서 음악을 출력하기 위해 PlaySound 함수를 사용함
// 함수를 실행하기 위해 필요함

using namespace sf;

const int M = 20;
const int N = 10;

int field[M][N] = { 0 };


struct Point
{
  int x, y;
} a[4], b[4];

int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5  // O
};

bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return 0;
		else if (field[a[i].y][a[i].x]) return 0;

	return 1;
};
	

int main()
{
	PlaySound(TEXT("jazz.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	// SND_FILENAME: 만약 파일을 발견하지 못했을 경우에 SND_NODEFAULT 플래그가 설정되어 있지 않다면 기본 소리가 재생된다
	// SND_ASYNC: 소리가 비동기로 연주되고 소리가 시작된 후 즉시 반환한다
	// SND_LOOP: PlaySound를 다시 호출하기 전까지 무한으로 반복된다
	
	srand(time(0));

	RenderWindow window(VideoMode(320, 480), "TETRIS!");

	Texture t;
	t.loadFromFile("images/tiles.png"); 

	Sprite s(t);
	s.setTextureRect(IntRect(0, 0, 18, 18));

	int dx = 0; bool rotate = 0; int colorNum = 1;
	float timer = 0, delay = 0.3;

	Clock clock;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();

			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::Up) rotate = true; // Up 키를 누르면 블록이 회전함
				else if (e.key.code == Keyboard::Left) dx = -1; // Left 키를 누르면 블록이 왼쪽으로 이동함
				else if (e.key.code == Keyboard::Right) dx = 1; // Right 키를 누르면 블록이 오른쪽으로 이동함
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.1; // Down 키를 누르면 블록이 0.1씩 빨라짐

		//// <- Move -> ////
		for (int i = 0; i < 4; i++) {
			b[i] = a[i];  a[i].x += dx;
		}
		if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

		//// Rotate ////
		if (rotate)
		{
			Point p = a[1]; // center of rotation
			for (int i = 0; i < 4; i++)
			{
				int x = a[i].y - p.y;
				int y = a[i].x - p.x;
				a[i].x = p.x - x;
				a[i].y = p.y + y;
			}
			if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];
		}
		// 블록이 프레임에 닿으면 게임 종료
		else if (!check())
		{
			break;
		}

		//// Tick ////
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++) {
				b[i] = a[i]; a[i].y += 1;
			}

			if (!check())
			{
				for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

				colorNum = 1 + rand() % 7; // 0 에서 7 까지 랜덤
				int n = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					a[i].x = figures[n][i] % 2;
					a[i].y = figures[n][i] / 2;
				}
			}
			timer = 0;
		}


		//// check lines ////
		int k = M - 1;
		for (int i = M - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < N; j++)
			{
				if (field[i][j]) count++;
				field[k][j] = field[i][j];
			}
			if (count < N) k--;
		}
		dx = 0; rotate = 0; delay = 0.3;


		//// draw ////
		window.clear(Color::White);
		Texture c;
		c.loadFromFile("images/background.png");

		Sprite v(c);
	    window.draw(v); // 배경이미지

		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
			{
				if (field[i][j] == 0) continue;
				s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
				s.setPosition(j * 18, i * 18);
				s.move(28, 31);
				window.draw(s);
			}

		for (int i = 0; i < 4; i++)
		{
			s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
			s.setPosition(a[i].x * 18, a[i].y * 18);
			s.move(28, 31);
			window.draw(s); // 이미지 그리기
		}

		window.display();
	}
	return 0;
}