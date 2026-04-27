# Tea Time Witch's Journey (티타임 마녀의 여행기)

> HD-2D 힐링 판타지 턴제 RPG. 마녀 Elia가 차를 만들며 동료들과 짧은 여정을 떠난다.

## 기술 스택
- **엔진**: Unreal Engine 5.4+
- **언어**: C++ (메인) + Blueprint (보조)
- **IDE**: JetBrains Rider (Visual Studio 2022 호환)
- **VCS**: Git + Git LFS
- **에셋**: AI 생성(Midjourney / SD) → 후처리 → import

## 빌드 방법

### 사전 준비
1. Unreal Engine 5.4 이상 설치 (Epic Games Launcher)
2. Visual Studio 2022 (Game development with C++) 또는 JetBrains Rider
3. Git LFS 설치 후 1회 `git lfs install`

### 빌드 단계
```bash
git clone <repo-url> Tea-Time-Witch
cd Tea-Time-Witch
git lfs pull
```
1. `TeaTimeWitch.uproject` 우클릭 → **Generate Visual Studio project files**
2. `TeaTimeWitch.sln` 또는 Rider에서 솔루션 열기
3. Configuration: **Development Editor** / Platform: **Win64**
4. Build → 성공 후 에디터 자동 실행

## 폴더 구조

```
Tea-Time-Witch/
├── Content/                       # 게임 에셋 (UE 자동 인식)
│   ├── Characters/                # Elia, Mocha, Aric, Enemies
│   ├── Maps/                      # TeaShop, Dungeon_Trench
│   ├── UI/                        # HUD, TeaCraftUI, BattleUI, DialogueUI
│   ├── Data/                      # DataTables, DataAssets
│   ├── Audio/                     # BGM, SFX
│   ├── FX/                        # Niagara, Materials
│   ├── Art/                       # AI 워크플로우 전용
│   │   ├── AI_Generated/          # AI 원본
│   │   └── Processed/             # 후처리본 (import 직전)
│   └── Blueprints/                # BP_GameMode, BP_PlayerController, ...
├── Source/                        # C++
│   └── TeaTimeWitch/
│       ├── Core/                  # GameMode, GameInstance, PlayerController
│       ├── Battle/                # BattleSystem, TurnManager, Skill
│       ├── TeaCraft/              # CraftingComponent, Recipe
│       ├── Dialogue/              # DialogueSystem, DialogueNode
│       └── UI/                    # HUD widget classes
├── Config/                        # *.ini 설정
└── TeaTimeWitch.uproject
```

## 개발 일정 (수직 슬라이스, 2일)
- **Block 1**: 프로젝트 셋업 (1h) ← 현재
- **Block 2~N**: 전투 / TeaCraft / Dialogue / 통합 / 폴리싱

## 라이선스
개인 포트폴리오용 (비상업).
