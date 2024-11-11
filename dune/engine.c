#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
void place_object(int, int, int, OBJECT_SAMPLE);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)  //일정한 간격 (TICK = 10ms)마다 증가
CURSOR cursor = { { 1, 1 }, {1, 1} };   //직전위치, 현재위치


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,   //현재 보유한 스파이스
	.spice_max = 9,  //스파이스 최대 저장량
	.population = 0,   //현재 인구 수
	.population_max = 0   //최대 인구 수
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',  //화면에 표시될 문자
	.speed = 300,
	.next_move_time = 300
};

OBJECT_SAMPLE h_base = {
	.pos = {1, MAP_WIDTH - 3},
	.dest = {0, 0},
	.repr = 'B',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_plate = {
	.pos = {1, MAP_WIDTH - 5},
	.dest = {0, 0},
	.repr = 'P',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_spice = {
	.pos = {MAP_HEIGHT - 11, MAP_WIDTH - 2},
	.dest = {0, 0},
	.repr = 'S',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_harvester = {
	.pos = {3, MAP_WIDTH - 2},
	.dest = {0, 0},
	.repr = 'H',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock1 = {
	.pos = {3, MAP_WIDTH - 34},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock2 = {
	.pos = {4, MAP_WIDTH - 12},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock3 = {
	.pos = {9, MAP_WIDTH - 44},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock4 = {
	.pos = {11, MAP_WIDTH - 30},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock5 = {
	.pos = {12, MAP_WIDTH - 9},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_base = {
	.pos = {MAP_HEIGHT - 3, 1},
	.dest = {0, 0},
	.repr = 'B',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_plate = {
	.pos = {MAP_HEIGHT - 3, 3},
	.dest = {0, 0},
	.repr = 'P',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_spice = {
	.pos = {MAP_HEIGHT - 9, 1},
	.dest = {0, 0},
	.repr = 'S',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_harvester = {
	.pos = {MAP_HEIGHT - 4, 1},
	.dest = {0, 0},
	.repr = 'H',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE sandwarm1 = {
	.pos = {2, MAP_WIDTH - 48},
	.dest = {0, 0},
	.repr = 'W',  //화면에 표시될 문자
	.speed = 300,
	.next_move_time = 300
};

OBJECT_SAMPLE sandwarm2 = {
	.pos = {10, MAP_WIDTH - 16},
	.dest = {0, 0},
	.repr = 'W',  //화면에 표시될 문자
	.speed = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));
	int is_update_requested = 0;
	int reset = 0;

	init();  //맵과 지형 생성
	intro();  // "DUNE 1.5"
	display(resource, map, cursor, is_update_requested, reset);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {   //방향키인지 확인하는 함수
			cursor_move(ktod(key));   //맞다면 커서 이동  Ktod: 키를 방향으로 변환
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro(); break;
			case k_none: break;
			case k_undef: break;
			case k_space:is_update_requested = 1; break;
			case k_escape:reset = 1;  break;
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor, is_update_requested, reset);
		is_update_requested = 0;
		reset = 0;
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void place_object(int layer, int width, int height, OBJECT_SAMPLE obj) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[layer][obj.pos.row + i][obj.pos.column + j] = obj.repr;
		}
	}
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';   //위 벽
		map[0][MAP_HEIGHT - 1][j] = '#';   //아래 벽
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';  //왼쪽 세로벽
		map[0][i][MAP_WIDTH - 1] = '#';    //오른쪽 세로벽


		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';   //가운데 빈 공간
		}
	}

	//하코넨 본진
	place_object(0, 2, 2, h_base);

	//하코넨 장판
	place_object(0, 2, 2, h_plate);

	//하코넨 스파이스
	place_object(0, 1, 1, h_spice);

	//rock
	place_object(0, 2, 2, rock1);
	place_object(0, 1, 1, rock2);
	place_object(0, 1, 1, rock3);
	place_object(0, 2, 2, rock4);
	place_object(0, 1, 1, rock5);

	//플레이어 본진
	place_object(0, 2, 2, p_base);

	//플레이어 장판
	place_object(0, 2, 2, p_plate);

	//플레이어 스파이스
	place_object(0, 1, 1, p_spice);

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	//하코넨 하베스터
	place_object(1, 1, 1, h_harvester);

	//플레이어 하베스터
	place_object(1, 1, 1, p_harvester);

	//샌드웜
	place_object(1, 1, 1, sandwarm1);
	place_object(1, 1, 1, sandwarm2);

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	static KEY last_key = k_none;            // 마지막으로 입력된 키 저장
	static int last_key_press_time = 0;      // 마지막 키 입력 시간을 기록
	int current_time = sys_clock;            // 현재 시간

	POSITION new_pos;

	if ((current_time - last_key_press_time) <= 150) {
		new_pos = pmove(cursor.current, dir);
		new_pos = pmove(new_pos, dir);
		new_pos = pmove(new_pos, dir);
	}
	else {
		new_pos = pmove(cursor.current, dir);
	}

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}

	// 마지막 키와 시간 갱신
	last_key_press_time = current_time;

}


/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);  //현재 위치와 목적지 간의 차이
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정 (왼쪽 위 -> 오른쪽 아래)
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	//목적지에 도달하지 않은 경우, 세로거리와 가로거리 중 큰 쪽을 기준으로 이동방향 결정
	if (abs(diff.row) >= abs(diff.column)) {    //세로가 더 크면 세로로 이동
		dir = (diff.row >= 0) ? d_down : d_up;  
	}
	else {  //아니라면 가로로 이동
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	
	//맵 경계 안에 있는지 확인
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {  // 해당 위치에 장애물이 있는지 없는지 확인

		return next_pos;  //조건 만족 시 위치 이동
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;     //현재 객체가 위치한 좌표의 값은 -1로 설정
	obj.pos = sample_obj_next_position();   //다음으로 이동할 위치 계산(객체가 이동 가능한지 확인 후 이동할 위치 반환)
	map[1][obj.pos.row][obj.pos.column] = obj.repr;   //객체가 새롭게 이동한 위치에 객체를 설정

	obj.next_move_time = sys_clock + obj.speed;  //
}