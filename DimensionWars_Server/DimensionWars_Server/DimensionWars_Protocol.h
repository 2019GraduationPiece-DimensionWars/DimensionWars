#pragma once

// 서버 프로그램인데 이게 없으면 안 되겠지요.
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <DirectXMath.h>

const unsigned short int const SERVERPORT = 3500;	// 서버 포트
const unsigned short int const BUFSIZE = 1024;		// 버퍼의 크기

constexpr unsigned char const MAX_USER = 6;	// 한 방과 한 전투에 플레이어는 6명. 이 게임은 투사체가 많지 플레이어가 많은 것이 아니다.
const unsigned char const MAX_PLAYER = MAX_USER;	// 코딩 할 때 불편하지 않도록 같은 이름을 쓰기 위함

constexpr unsigned int const MAX_OBJECTS = 1000;		// 총 서버가 관리할 플레이어, 투사체 등의 정보를 포함한 게임 월드의 모든 오브젝트 숫자
// 플레이어 6명, 큐브 50개, 투사체 X개 

const float const WORLD_HORIZONTAL = 25000.0f; // 월드의 가로
const float const WORLD_WIDTH = WORLD_HORIZONTAL;	// 코딩 할 때 불편하지 않도록 같은 이름을 쓰기 위함
const float const WORLD_HEIGHT = 25000.0f;		// 월드의 세로
const float const WORLD_VERTICAL = 25000.0f;	// 월드의 높이

const float const VIEW_RANGE = 2500.0f;	// 플레이어의 시야


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
	char id;		// client의 ID
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

struct CSPacket_Base {
	char size;
	char type;
};

struct CSPacket_Move : CSPacket_Base {
	unsigned char dir;	// Moving_Direction
};

struct CSPacket_Attack : CSPacket_Base {
	unsigned char attack_type;	// 어떤 평타인지, 스킬인지. 이 값의 경우 각 플레이어 캐릭터에 정의된 enum state 값을 집어넣는다.
};

#pragma pack(pop)

// value를 minimum, maximum 사이로 보정하는 함수
template <typename var>
var clamp(var minimum, var value, var maximum) { return  (minimum > ((value < maximum) ? value : maximum)) ? minimum : ((value < maximum) ? value : maximum); }
