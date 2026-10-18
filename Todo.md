# Todo

코드 작업 목록. 에디터/BP 작업은 `MyTodo.md`에 적는다.
기준: 2026-10-19 전체 리뷰.

## 1. 인코딩 ✅

- [x] UTF-8 BOM 변환, 드러난 누락 include 수정

## 2. 복구 (한 판이 끝까지 돌게 만들기)

- [x] 골드 획득원: `ADEPickup_Gold` 추가, Greed 적용 (`d078cab`). 인게임 확인은 MyTodo 참고
- [x] 골드 정산/저장을 `UDEResultWidget`에서 `ADEGameMode_Stage::GameOver`로 이동 (`7d265ec`)
- [x] 보스 사망 바인딩: `SpawnBoss`가 `SpawnFromPool`을 거치도록 변경. 바인딩, 높이 보정, 풀 반환이 일반 몬스터와 같아짐
- [ ] 픽업: `UDEPickupManager::ActivePickups`가 획득 후에도 제거되지 않아, 누적 400개가 되면 드롭이 중단된다
- [ ] 풀: `UDEPoolSubsystem::ReturnActorToPool`에 중복 반환 검사가 없다. AcidBolt, `CancelSkill` 등에서 이중 반환
- [ ] `GameOver` 중복 호출 가드 (`bIsGameOver`). 두 번 불리면 골드가 두 번 정산된다. 무적 해제할 때 같이
- [ ] 메타 업그레이드 Amount: `ApplyMetaUpgrades`가 Amount modifier의 `Multiplier`를 안 넣어서 기본값 1.0이 들어간다. 업그레이드 1레벨 이상이면 Amount ×2 (`FDEStatModifier::Multiplier` 기본값 버그의 실사례)

## 3. 죽은 코드 삭제 (BP 참조 확인 후)

- [ ] Test* 클래스, 템플릿 GameMode/PC, `DEMonsterDataAsset`, `DESimpleMeleeBase`, `DEBloodyNail` 액터, 빈 파일, 대량으로 주석 처리된 옛 구현
- [ ] `ADEMonsterSpawnManager`의 옛 웨이브 시스템 잔재: `MonsterClasses`, `MonsterPool`, `InitialPoolSize`, `SpawnInterval`, `MonstersPerWave`, `WaveInterval`, `TotalWaves`, `CurrentWave`, `SpawnedThisWave`, `SpawnTimerHandle`, `WaveTimerHandle` (C++ 사용처 0, BP 노출 없음. 레벨 인스턴스에 값만 남아 있을 수 있음)
- [ ] `ADEMonsterSpawnManager`: `bIsResolvingOverlaps`(쓰기만 하고 안 읽음), `KillCount`(증가만 하고 안 읽음), `MonsterHISM`(인스턴스 추가 없음. 생성자에서 RootScene을 덮어써서 RootScene이 고아가 됨)

## 4. 경로 통일

- [ ] 데미지 경로
- [ ] 일시정지와 입력 모드 관리를 한 곳으로
- [ ] 스턴 2개 → StatusEffect 하나로
- [ ] 레벨업 큐 (다중 레벨업, 선택지 0개 처리)
- [ ] 킬 처리를 `FDEDamageResult` 기반으로 일원화 (킬 카운터 중복 해소)
- [ ] `EvolveSkill`이 `EndSkill` 대신 `CancelSkill`을 쓰도록
- [ ] 레거시 스킬을 BP 조립 방식으로 이전

## 웨이브/스폰 (개발자가 직접 정리 예정, Claude는 요청 시에만)

- [ ] 웨이브 타입이 `Duration` 부호로 인코딩됨 (0=OneShot, >0=Timed, <0=Infinite). `EWaveType` enum은 선언만 있고 `FDEStageWaveData`에 필드가 없다
- [x] `MinimumCount`: 해당 웨이브 몬스터가 맵에 최소 이만큼은 항상 있어야 한다. 적으면 즉시 보충. `CurrentWaveMonsters`(웨이브 시작 시 초기화)로 추적, `ReplenishToMinimum`으로 시작 버스트와 보충을 같이 처리. OneShot은 시작 버스트만, Timed는 Duration 동안, Infinite는 다음 웨이브 전까지 유지. SpawnLimit 준수
- [ ] 주기 스폰은 현재 웨이브만 한다. 다음 웨이브가 시작되면 이전 Timed/Infinite 웨이브는 즉시 끊긴다 (의도인지 확인)
- [ ] 시계 두 개: `WaveElapsedTime`은 DeltaTime 누적, `NextSpawnTime`은 GameMode 절대시간
- [ ] 매 Tick `FindRow`. BeginPlay에서 정렬할 때 row 포인터를 캐시하면 된다
- [x] `StartWave`의 버스트 루프가 `TrySpawnMonster`와 로직 중복 (`ReplenishToMinimum`으로 통합)
- [x] `TrySpawnMonster`가 `SpawnFromPool` 실패(nullptr)여도 true 반환
- [x] 데이터 ID 오타 시 0.2초마다 Error 로그 반복 (BeginPlay에서 한 번만 검사하도록 변경)
- [x] `GetRandomSpawnLocation`: 대각선 네 모서리에서만 스폰되던 문제. 안쪽 사각형에 걸리면 다시 뽑는 방식으로 수정 (Z=0 고정은 그대로)
- [x] 보스는 SpawnLimit 대상이 아니다 (원 기획). `ActiveBosses`로 따로 세서 SpawnLimit 계산에서 제외
- [ ] `Player`를 BeginPlay에서 `GetPlayerController(0)`으로 한 번만 캐시. Pawn이 늦게 빙의되면 `Player`가 null로 남아 Tick이 통째로 스킵된다
