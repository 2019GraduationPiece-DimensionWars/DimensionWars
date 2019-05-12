#pragma once

// 서버 프로그램인데 이게 없으면 안 되겠지요.
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <DirectXMath.h>

constexpr unsigned short SERVERPORT = 3500;	// 서버 포트
constexpr unsigned short BUFSIZE = 1024;		// 버퍼의 크기

constexpr unsigned short MAX_USER = 500;	// 한 방과 한 전투에 플레이어는 6명. 이 게임은 투사체가 많지 플레이어가 많은 것이 아니다.
constexpr unsigned short Cube_start = 450;
constexpr unsigned short MAX_PLAYER = 6;	// 코딩 할 때 불편하지 않도록 같은 이름을 쓰기 위함

constexpr unsigned int MAX_OBJECTS = 1000;		// 총 서버가 관리할 플레이어, 투사체 등의 정보를 포함한 게임 월드의 모든 오브젝트 숫자
// 플레이어 6명, 큐브 50개, 투사체 X개 

constexpr float WORLD_HORIZONTAL = 25000.0f; // 월드의 가로
constexpr float WORLD_WIDTH = WORLD_HORIZONTAL;	// 코딩 할 때 불편하지 않도록 같은 이름을 쓰기 위함
constexpr float WORLD_HEIGHT = 25000.0f;		// 월드의 세로
constexpr float WORLD_VERTICAL = 25000.0f;	// 월드의 높이

constexpr float VIEW_RANGE = 2500.0f;	// 플레이어의 시야

constexpr float MAX_CUBE_SIZE = 700.0f;	// 큐브 사이즈


namespace GrimReaper
{
	// 0번 Full 애니메이션은 테스트용으로 FBX 내부의 모든 애니메이션을 순회하며 재생하는 것, 실전에선 필요 없음

	enum GrimReaperState {	// 사신 애니메이션 패킷
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Hide_Invasion = 5,
		// Full_Attack = 6,
		First_Attack = 6,
		Second_Attack = 7,
		Third_Attack = 8,
		Slash_Wave = 9,
		Beheading = 10,
		Move_Forward = 11,
		Move_Right = 13,	//Move_Right = 12,
		Move_Left = 12,		//Move_Left = 13,
		Move_Backward = 14,
		Down = 15
	};
}

namespace Gambler
{
	enum GamblerState {	// 도박사 애니메이션 패킷
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		Shuffle = 5,
		Idle_Attack = 6,
		Multi_Shot = 7,
		Wild_Card = 8,
		Jump = 9,
		Jump_Attack = 10,
		Fall = 11,
		Fall_Attack = 12,
		Move_Forward = 13,
		Move_Forward_Attack1 = 14,
		Move_Forward_Attack2 = 15,
		Move_Right_Forward = 19,			// Move_Right_Forward = 16,
		Move_Right_Forward_Attack1 = 20,	// Move_Right_Forward_Attack1 = 17,
		Move_Right_Forward_Attack2 = 21,	// Move_Right_Forward_Attack2 = 18,
		Move_Left_Forward = 16,				// Move_Left_Forward = 19,
		Move_Left_Forward_Attack1 = 17,		// Move_Left_Forward_Attack1 = 20,
		Move_Left_Forward_Attack2 = 18,		// Move_Left_Forward_Attack2 = 21,
		Move_Right = 25,					// Move_Right = 22,
		Move_Right_Attack1 = 26,			// Move_Right_Attack1 = 23,
		Move_Right_Attack2 = 27,			// Move_Right_Attack2 = 24,
		Move_Left = 22,						// Move_Left = 25,
		Move_Left_Attack1 = 23,				// Move_Left_Attack1 = 26,
		Move_Left_Attack2 = 24,				// Move_Left_Attack2 = 27,
		Move_Backward = 28,
		Move_Backward_Attack1 = 29,
		Move_Backward_Attack2 = 30,
		Move_Right_Backward = 34,			//Move_Right_Backward = 31,
		Move_Right_Backward_Attack1 = 35,	//Move_Right_Backward_Attack1 = 32,
		Move_Right_Backward_Attack2 = 36,	//Move_Right_Backward_Attack2 = 33,
		Move_Left_Backward = 31,			//Move_Left_Backward = 34,
		Move_Left_Backward_Attack1 = 32,	//Move_Left_Backward_Attack1 = 35,
		Move_Left_Backward_Attack2 = 33,	//Move_Left_Backward_Attack2 = 36,
		Down = 37
	};
}

namespace ElfArcher
{
	enum ElfArcherState {	// 도박사 애니메이션 패킷
		Full = 0,
		Idle = 1,
		OnHit = 2,
		Guard = 3,
		Burf = 4,
		First_Shot = 5,
		Second_Shot = 6,
		Third_Shot = 7,
		Guide_Shot = 8,
		Snipe = 9,
		Jump = 10,
		Jump_Attack = 11,
		Fall = 12,
		Fall_Attack = 13,
		Move_Forward = 14,
		Move_Right_Forward = 16,	// Move_Right_Forward = 15,
		Move_Left_Forward = 15,		// Move_Left_Forward = 16,
		Move_Right = 18,			// Move_Right = 17,
		Move_Left = 17,				// Move_Left = 18,
		Move_Backward = 19,
		Move_Right_Backward = 21,	// Move_Right_Backward = 20,
		Move_Left_Backward = 20,	// Move_Left_Backward = 21,
		Down = 22
	};
}

using ReaperState = GrimReaper::GrimReaperState;
using GamblerState = Gambler::GamblerState;
using ElfState = ElfArcher::ElfArcherState;


// 플레이어의 키 입력 정보를 분석하기 위한 일종의 define 고정값. 6비트만 사용
enum Key_Moving_Direction {
	DIR_FORWARD = 0x01,
	DIR_BACKWARD = 0x02,
	DIR_LEFT = 0x04,
	DIR_RIGHT = 0x08,
	DIR_UP = 0x10,	// 사신만 사용하는 고도 상승
	DIR_DOWN = 0x20 // 추락
};

namespace SC 
{
	enum ServerToClientSocketType {
		LoginOK = 1,
		PutPlayer = 2,
		RemovePlayer = 3,
		Position = 4,
		OnHit = 5, // 해당 클라이언트가 충돌당했으므로 경직 애니메이션을 재생해라.
		Down = 6, // 해당 클라이언트가 다운 공격에 피격되었으므로 피격 애니메이션을 재생해라.
		MapInfo =7,
		Count

	};
}
using SC_Type = SC::ServerToClientSocketType;	// 약자 형태로 사용할 것이다.

namespace CS 
{
	enum ClientToServerSocketType {
		Move = 1,
		Attack = 2,
		Guard = 3,
		Burf = 4,
	};
}
using CS_Type = CS::ClientToServerSocketType;	// 약자 형태로 사용할 것이다.

#pragma pack(push, 1)
// 구조체 상속을 적극 활용하여 중복된 코드의 길이를 줄임.
struct SCPacket_Base {
	char size;
	char type;
	unsigned short id;		// client의 ID
};

using SCPacket_LoginOK = SCPacket_Base; // Login OK 패킷은 기본 패킷과 구성 요소가 동일하므로 단순 Using

struct SCPacket_Position : SCPacket_Base {
	// 이 id를 가진 객체를 해당 좌표로
	DirectX::XMFLOAT3 position;
	
};

struct SCPacket_PutPlayer : SCPacket_Position {
	// 플레이어가 어떤 캐릭터인가 등의 정보를 추가
};

struct SCPacket_RemovePlayer : SCPacket_Base {
	// 이 id를 가진 객체를 화면에서 지운다
};

struct SCPacket_MapInfo : SCPacket_Base {
	DirectX::XMFLOAT3 position; // 위치정보
	DirectX::XMFLOAT3 rotate; // 회전정보


	// 맵의 기본 정보
};

struct CSPacket_Base {
	char size;
	char type;
};

struct CSPacket_Move : CSPacket_Base {
	unsigned char dir;	// Moving_Direction
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Look;

};

struct CSPacket_Attack : CSPacket_Base {
	unsigned char attack_type;	// 어떤 평타인지, 스킬인지. 이 값의 경우 각 플레이어 캐릭터에 정의된 enum state 값을 집어넣는다.
};

#pragma pack(pop)

// value를 minimum, maximum 사이로 보정하는 함수
template <typename var>
var clamp(var minimum, var value, var maximum) { return  (minimum > ((value < maximum) ? value : maximum)) ? minimum : ((value < maximum) ? value : maximum); }
