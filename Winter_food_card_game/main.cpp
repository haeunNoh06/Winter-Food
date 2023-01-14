#include <stdio.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>

using namespace sf;

const int row = 4;
const int CARD_W = 200;
const int CARD_H = 200;

struct Card {
	RectangleShape sprite;
	int id_i;
	int id_j;
	int type;
	int is_clicked;// 눌렸는가
	int is_cleared;// 정답을 맞춘 카드인가
};

struct Textures {
	Texture img[8 + 1];
	Texture gameclear;
};

struct SBuffer {
	SoundBuffer flipped_card;// 카드 뒤집는 소리
};

struct Gameclear {
	Sprite sprite;
};

void swap_card(struct Card* c1, struct Card* c2)
{
	struct Card tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}

int main(void) {

	RenderWindow window(VideoMode(1200, 800), "AfterSchool2");// 화면 크기, 제목
	window.setFramerateLimit(60);// 60초마다 한 번씩 돌아간다

	// f : 소수
	// Vector2i : 정수
	Vector2i mouse_pos;// 마우스 좌표
	int flipped_num;// 현재 뒤집혀진 카드의 갯수
	int cleared_num;// 정답인 카드 갯수

	long start_time;// 프로그램 시작 시각
	long spent_time;// 현재 시각
	long delay_time;// 바로 다시 ? 로 뒤집혀지지 않도록 딜레이를 줌

	struct Gameclear clear;
	int gameclear;// 게임 종료 여부
	int start = 1;// 게임 다시 시작 여부

	srand(time(0));

	struct Textures t;
	t.img[0].loadFromFile("./resources/images/000.png");
	t.img[1].loadFromFile("./resources/images/001.png");
	t.img[2].loadFromFile("./resources/images/002.png");
	t.img[3].loadFromFile("./resources/images/003.png");
	t.img[4].loadFromFile("./resources/images/004.png");
	t.img[5].loadFromFile("./resources/images/005.png");
	t.img[6].loadFromFile("./resources/images/006.png");
	t.img[7].loadFromFile("./resources/images/007.png");
	t.img[8].loadFromFile("./resources/images/008.png");
	t.gameclear.loadFromFile("./resources/images/gameclear.png");

	struct SBuffer sb;
	sb.flipped_card.loadFromFile("./resources/sounds/flipped_card.ogg");

	// 카드 뒤집기
	Sound flip_sound;
	flip_sound.setBuffer(sb.flipped_card);

	Font font;
	font.loadFromFile("c:/Windows/Fonts/arial.ttf");

	// 텍스트
	Text text;
	text.setFont(font);
	text.setCharacterSize(30);// 글자 크기
	text.setFillColor(Color::Black);
	text.setPosition(0, 0);
	char info[40];

	struct Card compare_card;// 첫 번째로 뒤집힌 카드
	struct Card cards[row][row];
	int n;

	while (window.isOpen())
	{
		if (start == 1)
		{
			gameclear = 0;// 게임 클리어 여부
			flipped_num = 0;// 현재 뒤집혀진 카드의 갯수
			cleared_num = 0;// 정답인 카드 갯수
			n = 0;
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < row; j++)
				{
					cards[i][j].sprite.setSize(Vector2f(CARD_W, CARD_H));
					cards[i][j].sprite.setPosition(j * CARD_W, i * CARD_H);// j가 커질수록 x값이 100증가, i가 커질수록 y값이 200증가
					cards[i][j].sprite.setTexture(&t.img[0]);// 뒤집는 그림이 0이기 때문에 1부터
					cards[i][j].type = 1 + n / 2;
					// id초기화 없어짐, 섞은 후 id값은 나중에 주기
					cards[i][j].is_clicked = 0;
					cards[i][j].is_cleared = 0;// 아직 깨지 않았으므로 0임
					n++;
				}
			}

			// 카드 100번 섞기
			for (int i = 0; i < 100; i++)
			{
				swap_card(&cards[rand() % row][rand() % row], &cards[rand() % row][rand() % row]);
			}

			// 인덱스에 맞춰id값, 위치 재조정
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < row; j++)
				{
					cards[i][j].id_i = i;
					cards[i][j].id_j = j;
					cards[i][j].sprite.setPosition(j * CARD_W + 200, i * CARD_H);
				}
			}

			start_time = clock();
			delay_time = start_time;
		}
		start = 0;

		mouse_pos = Mouse::getPosition(window);// 마우스 좌표 실시간으로 받기
		if (gameclear == 0)
			spent_time = clock() - start_time;

		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
				// 한 번 누르면 한 번만 적용
			case Event::MouseButtonPressed:
				if (event.mouseButton.button == Mouse::Left)
				{
					for (int i = 0; i < row; i++)
					{
						for (int j = 0; j < row; j++)
						{
							// 마우스 위치가 cards[i][j]의 위치에 해당한다면?
							if (cards[i][j].sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y))
							{
								// 뒤집히지 않은 카드와 뒤집어진 카드 수가 2개 미만일 때만 뒤집겠다
								if (cards[i][j].is_clicked == 0 && flipped_num < 2)
								{
									flip_sound.play();// 카드 뒤집으면 효과음
									cards[i][j].is_clicked = 1;// true
									flipped_num++;// 뒤집혀진 카드 갯수

									// 뒤집혀진 카드가 한 개라면
									if (flipped_num == 1)
									{
										compare_card = cards[i][j];// 첫 번째 카드를 비교카드로 만듦
									}
									// 두 개를 뒤집었다면
									else if (flipped_num == 2)
									{
										// 만약 두 카드가 같은 종류면
										if (compare_card.type == cards[i][j].type)
										{
											// 현재 카드와 기준카드가 같으면 둘 다 정답임을 나타냄
											cards[i][j].is_cleared = 1;
											cards[compare_card.id_i][compare_card.id_j].is_cleared = 1;
											cleared_num += 2;// 정답인 카드 2개 추가
										}
										// 두 카드가 다른 종류이면
										else
										{
											cards[i][j].is_clicked = 1;
											delay_time = spent_time;
										}

									}
								}
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < row; j++)
			{
				// 클릭이 된 상태이거나 정답을 맞춘 카드이면
				if (cards[i][j].is_clicked == 1 || cards[i][j].is_cleared == 1)
				{
					// 그림이 있는 스프라이트로 변경 (카드를 뒤집겠다는 의미)
					cards[i][j].sprite.setTexture(&t.img[cards[i][j].type]);
				}
				else
				{
					// 카드는 ??? 상태
					cards[i][j].sprite.setTexture(&t.img[0]);
				}
			}
		}

		// 뒤집힌 카드가 2개라면
		if (flipped_num == 2)
		{
			// 두 카드가 뒤집힌지 1초 이내가 아니라면
			if (spent_time - delay_time > 1000)
			{
				for (int i = 0; i < row; i++)
					for (int j = 0; j < row; j++)
						cards[i][j].is_clicked = 0;
				// 다 뒤집힌 상태로 만들어 버리겠다
				flipped_num = 0;
			}
		}

		sprintf(info, " time: %d\n", spent_time / 1000);
		text.setString(info);

		window.clear(Color::White);

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < row; j++)
			{
				window.draw(cards[i][j].sprite);
			}
		}

		// 게임 종료
		if (cleared_num == 16)
		{
			gameclear = 1;
			clear.sprite.setTexture(t.gameclear);
			window.draw(clear.sprite);
		}

		// 재시작
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			start = 1;
			window.clear(Color::White);
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < row; j++)
				{
					window.draw(cards[i][j].sprite);
				}
			}
		}

		window.draw(text);

		window.display();

	}
	return 0;
}