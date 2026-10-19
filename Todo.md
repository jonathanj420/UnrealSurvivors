# Todo

코드 작업 목록. 에디터/BP 작업은 `MyTodo.md`에 적는다.
기준: 2026-10-19 전체 리뷰.

## 1. 인코딩 ✅

- [x] UTF-8 BOM 변환, 드러난 누락 include 수정

## 2. 복구 (한 판이 끝까지 돌게 만들기)

- [x] 골드 획득원: `ADEPickup_Gold` 추가, Greed 적용 (`d078cab`). 인게임 확인은 MyTodo 참고
- [x] 골드 정산/저장을 `UDEResultWidget`에서 `ADEGameMode_Stage::GameOver`로 이동 (`7d265ec`)
- [x] 보스 사망 바인딩: `SpawnBoss`가 `SpawnFromPool`을 거치도록 변경. 바인딩, 높이 보정, 풀 반환이 일반 몬스터와 같아짐
- [x] 픽업 장부 누적: `DeactivatePickup`에서 `UnregisterPickup`을 호출해 `ActivePickups`/`MagnetizingPickups`에서 제거. 풀 재사용 시 중복 등록되던 문제도 같이 해결
- [x] ~~픽업 자석 범위 감지가 `bCanBeMagnetized`를 안 본다~~ 의도된 동작. 플래그는 전역 자석 전용 (VS의 Vacuum처럼 보석류만), 범위 자석은 모든 픽업 대상
- [x] 풀 중복 반환: `PooledActorSet`(TObjectKey)으로 이미 풀에 있는 액터의 재반환을 막고 Warning 로그 (`[Pool] Duplicate return ignored`)
- [x] 풀 stale 반환: `CancelSkill`이 반환하기 전에 `ADESkillActorBase::IsActiveFor(this)`(숨김 상태가 아니고 `CachedContext.SourceSkill`이 자기 자신)를 확인한다. 재사용될 때 `InitializeFromContext`가 SourceSkill을 덮어쓰므로 남의 액터는 건너뛴다
- [ ] AcidBolt 이중 반환 경로 확인: `Explode`가 한 번 반환된 뒤 다시 불리는 경로가 있는지 (같은 프레임 다중 오버랩 등). 가드 로그로 재현 확인 후 원인 수정
- [x] AOE(Darkness/GreyDarkness/DanseMacabre)가 몬스터를 못 맞히던 문제: 오버랩은 양쪽 모두 `GenerateOverlapEvents`가 켜져 있어야 하는데 몬스터 캡슐과 `ADESkillActorBase` 구체 둘 다 꺼져 있었다. `ADESimpleAOEBase`가 오버랩 이벤트 대신 `OverlapMultiByObjectType`(Monster)으로 직접 쿼리하도록 변경. 장판은 HitInterval마다 한 번 범위 전체 타격, Orbit은 `bHitOnContact`로 매 Tick 쿼리 + 타겟별 쿨다운. SpatialGrid로 갈 때 `QueryTargets` 한 곳만 바꾸면 된다
- [x] `DarkEdenSurvivors.h`의 채널 매크로 수정: `ECC_Monster`가 `GameTraceChannel2`(DECharacter), `ECC_PlayerAttack`가 `GameTraceChannel3`(Attack 트레이스)로 잘못 정의되어 있었다. ini 기준 5/6으로 고치고, 숫자를 직접 쓰던 6곳도 매크로로 통일. `UDEGameplayLibrary::GetNearestTarget`/`GetRandomTargets`의 반경 쿼리 경로가 몬스터를 못 찾던 문제 해결 (C++ 호출처는 전부 반경 -1이라 드러나지 않았음)
- [ ] `GameOver` 중복 호출 가드 (`bIsGameOver`). 두 번 불리면 골드가 두 번 정산된다. 무적 해제할 때 같이
- [ ] 몬스터 접촉 공격을 `UGameplayStatics::ApplyDamage`에서 `ApplyCombatDamage`로 (`ADEMonsterBase::ExecuteAttackLogic`). `ApplyCombatDamage`는 `ProcessDamage`를 직접 불러 무적(`TakeDamage`)을 우회하므로 무적 해제할 때 같이
- [x] 메타 업그레이드 Amount ×2: `FDEStatModifier::Multiplier` 기본값을 0으로 변경. 액세서리 DataAsset은 Multiplier를 전부 명시적으로 저장해 두어서 값이 그대로다 (Buman 0.1, 나머지 0). 다른 에셋 사용처 없음

## 3. 죽은 코드 삭제 (BP 참조 확인 후)

- [x] `DEBloodyNail` 액터 + `DESimpleMeleeBase` 삭제. `DESkill_BloodyNail.cpp`의 주석 처리된 옛 `ActivateSkill`도 같이 삭제. 개발자가 삭제 승인 (2026-10-19). 근접 스킬은 이제 스윕 판정이라 sphere 액터 방식은 안 쓴다. 둘은 서로만 참조함(`ADEBloodyNail : ADESimpleMeleeBase`), 다른 C++ 참조는 `DESkill_BloodyNail.cpp`의 주석 한 줄뿐, `.uasset` grep 결과 0. `DESkill_BloodyNail`(스킬 클래스)은 별개라 삭제 대상 아님
- [x] Test* 클래스 4개 삭제 (`TestAIController`, `TESTHud`, `TestProjectile`, `TestStatComponent`). 개발자가 미사용 확인
- [x] 템플릿 정리: `ADarkEdenSurvivorsGameMode`/`PlayerController`/`Character`, `TP_ThirdPerson*` 리다이렉트, `Variant_*` C++와 Build.cs include 경로, 빈 `DEGameMode.cpp` 삭제. 기본 GameMode는 `BP_DEGameMode_Stage`로 변경 (개발자). git 미추적 콘텐츠(`Content/ThirdPerson`, `Content/Variant_*`, 옛 기본 GameMode/`BP_TESTHud`/`BP_TestCharacter`)는 `../DarkEdenSurvivors_DeletedBackup_20261019/`로 이동
- [x] Build.cs에서 안 쓰는 의존성 삭제: `InputCore`, `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `NavigationSystem`, `Slate`, `GameplayTasks`
- [x] `.uproject`에서 `StateTree`/`GameplayStateTree` 플러그인 비활성화 (템플릿 전용, StateTree 에셋 없음)
- [x] `DEMonsterDataAsset` 삭제 (몬스터 최적화용으로 만들었다가 지금의 DT + 중앙 업데이트 방식으로 대체됨)
- [x] 주석 처리된 옛 구현 삭제 (약 1,900줄). 보류 중인 기능이라 남긴 것: 몬스터 걷기 애니메이션(`DEMonsterBase`), 스턴 색 변경(`DEStatusEffect_Stun`), Darkness 넉백 누적(`DEAOE_Darkness`/`GreyDarkness`), 웨이브 Duration 체크(`DEMonsterSpawnManager`, 웨이브 영역)
- [x] `ADEMonsterSpawnManager`의 옛 웨이브 시스템 잔재 삭제: `MonsterClasses`, `MonsterPool`, `InitialPoolSize`, `SpawnInterval`, `MonstersPerWave`, `WaveInterval`, `TotalWaves`, `CurrentWave`, `SpawnedThisWave`, `SpawnTimerHandle`, `WaveTimerHandle` (C++ 사용처 0, BP 노출 없음. 레벨 인스턴스에 값만 남아 있을 수 있음)
- [x] `ADEMonsterSpawnManager` 삭제: `bIsResolvingOverlaps`(쓰기만 하고 안 읽음), `KillCount`(증가만 하고 안 읽음), `MonsterHISM`(인스턴스 추가 없음. 생성자에서 RootScene을 덮어써서 RootScene이 고아가 됨)

## 4. 경로 통일

- [ ] 데미지 경로
- [ ] 일시정지와 입력 모드 관리를 한 곳으로
- [ ] 스턴 2개 → StatusEffect 하나로
- [ ] 레벨업 큐: `AddExp`가 한 번에 여러 번 `OnLevelUp`을 쏘면 위젯이 여러 개 동시에 생기고, 전부 같은 상태로 선택지를 만든다. 첫 위젯을 닫으면 `ResumeGame`으로 나머지가 열린 채 게임이 재개된다. 선택지 생성은 위젯 `NativeConstruct`가 아니라 매니저가, 적용은 `LevelUpManager::ApplyChoice`를 거치게 (멀티 대비)
  - 선택지 0개일 때 멈춤은 임시로 `ShowLevelUpUI`에서 위젯을 닫고 일시정지를 건너뛰게 막아둠
- [ ] 킬 처리를 `FDEDamageResult` 기반으로 일원화 (킬 카운터 중복 해소)
- [x] `EvolveSkill`이 `EndSkill` 대신 `CancelSkill`을 쓰도록. `CancelSkill`은 `CachedContext.SpawnedAOEs`(Orbit, SpawnAOE)도 회수한다. 날아가는 투사체는 추적하지 않으므로 수명이 다할 때까지 남는다
- [ ] 레거시 스킬을 BP 조립 방식으로 이전

## 스킬 습득/레벨업/진화 (2026-10-20 리뷰)

- [x] 발동/Behavior마다 찍히던 로그 삭제 (`Activate`, `ExecutePipeline`, `SpawnAOE`, `PlayEffect` 빔). `SetSkillData`의 null 역참조 수정
- [x] 상자 `GetFallbackChoice()`의 nullptr이 보상 배열에 들어가던 문제
- [ ] "올릴 수 있나" 판정 일원화: `SkillManager`에 판정 함수를 하나 두고 레벨업 카드, 상자, `LevelUpSkill`(bool 반환)이 같이 쓰게. 지금은 슬롯이 꽉 차도 새 스킬 카드가 나오고 고르면 `LevelUpSkill`이 조용히 return해서 레벨업 하나가 사라진다. 최대 레벨 기준도 레벨업은 DT 행 존재, 상자는 `FDESkillRow::MaxLevel`로 다르다 (결정 필요: 어느 쪽을 기준으로)
- [ ] 상자 보상을 미리 다 뽑고 나중에 적용해서, 진화로 소모된 재료의 업그레이드가 뒤에 적용되면 재료가 Lv1로 다시 생긴다 (재료 `RequiredLevel` < MaxLevel일 때)
- [ ] 상태 일원화: 레벨이 `SkillLevels`/`FActiveSkill::CurrentLevel`/`UDESkillBase::CurrentLevel` 세 군데, 보유 여부가 `ActiveSkills`/`Inventory::OwnedSkillIDs` 두 군데. `ActiveSkills` 하나로 (결정 필요: Inventory 스킬 목록을 슬롯 순서용으로 남길지)
- [ ] 레벨업 시 쿨다운 0 리셋이 Delay 파이프라인 진행 중에 `Activate`를 다시 불러 이전 시전을 끊는다 (기획 판단 필요)
- [ ] 죽은 코드: `GetRandomSkillChoices`, `GetUpgradeableSkills`, `ApplyCharacterDamageMultiplier`, `DeltaCheck`, `TotalReducedCooldownAmount`, `AccessoryWeight`, `bCooldownAfterDuration`(set만 하고 안 읽음)
- [ ] 스킬 DT를 생성자의 하드코딩 경로로 로드해서 `SkillRowMap`을 만든다. `SkillRowTable`은 EditAnywhere라 BP에서 바꿔도 맵에 반영되지 않는다

## 몬스터 최적화 (목표 동시 1000마리)

- [x] 접촉 공격을 오버랩 이벤트 대신 거리 판정으로 (`6207bc9`)
- [x] 풀 재사용 시 같은 메시면 `SetSkeletalMesh` 생략, 캡슐은 크기가 바뀔 때만 갱신 (`577097c`)
- [x] 스폰 높이를 CDO 대신 데이터 `CapsuleHalfHeight`로 (`883ffac`)
- [x] 몬스터 캡슐을 내비게이션에서 제외 (`SetCanEverAffectNavigation(false)`). 테스트 맵에 NavMesh + `RuntimeGeneration=Dynamic`이 있어서 몬스터가 움직일 때마다 내비 옥트리 갱신이 일어났을 가능성. 측정으로 효과 확인 필요
- [ ] 렌더 설정: `r.RayTracing=True` 끄기 (사용처 없음, 스키닝 메시 RT 갱신 비용 가능성). `r.Substrate`, `r.GenerateMeshDistanceFields`도 사용처 확인 후 정리
- [ ] 측정: Standalone, `t.MaxFPS 0`, `r.VSync 0`, `stat unit`으로 300/600/1000마리. 300마리 60fps는 이전 측정(상한 여부 불명). 2026-10-19 300마리: Frame/Game 12.9ms, Draw 5.6, RHIT 7.0, GPU 5.5, Draws 1078 (게임 스레드 병목)
- [x] 측정 ① 내비 제외 (2026-10-20, 300마리): Game 12.80, Draw 6.11, RHIT 7.57, GPU 5.32. 효과 없음 (`Docs/aoe.png`)
- [x] 측정 ② 캡슐 `NoCollision` 임시 실험 (2026-10-20, 300마리, 원복함): Game 9.64 (-3.2ms), Draw 10.94, RHIT 7.18, GPU 5.41 (`Docs/nocolunit.png`). 콜리전(Chaos 바디 갱신) 비용은 300마리에 약 3ms. Draw가 6.1에서 10.9로 오른 이유는 불명 (에디터를 띄워둔 상태였음)
  - `stat game` (`Docs/nocolgame.png`): World Tick 6.48, Tick Time 6.15 (Ticks Queued 345, 몬스터 액터 Tick은 꺼져 있으니 대부분 SkeletalMesh 컴포넌트 Tick으로 추정, `stat anim`으로 확인 필요), Post Tick Component Update 3.13, Transform or RenderData 2.57 (616회 = 몬스터당 2개 컴포넌트), MoveComponent 1.19 (303회)
  - 결론: 남은 비용은 몬스터별 SkeletalMesh(애니메이션 Tick, 트랜스폼/렌더 데이터 갱신, 드로우)에 몰려 있다. SpatialGrid는 약 3ms, ISM + VAT가 더 큰 덩어리
- [ ] 측정 순서 (한 번에 하나씩 바꿔서 효과를 분리한다): ① 내비 제외 빌드(`f9ff2fc`)로 재측정 → ② 임시 실험: `DEMonsterBase.cpp`의 캡슐을 `NoCollision`으로 바꿔 Game ms가 얼마나 주는지 확인 (스킬 판정은 깨짐, 측정 후 원복) → ③ `r.RayTracing=False`. 숫자가 애매하면 Unreal Insights(`-trace=default`)로 GameThread 프로파일. ②에서 많이 줄면 SpatialGrid 쿼리를 먼저, 거의 안 줄면 ISM + VAT를 먼저
- [ ] 스킬 타겟 쿼리를 물리 씬 대신 SpatialGrid로, 몬스터 콜리전 끄기 (Chaos 바디 갱신 제거. 측정 ②에서 300마리 약 3ms)
  - [x] `UDEMonsterQuerySubsystem`(WorldSubsystem)에 그리드 이동. `UDEMonsterUpdateComponent::ResolveOverlaps`가 `RebuildGrid`로 채우고 겹침 해소에도 그대로 쓴다. 2D 쿼리 `QueryCircle`/`QueryNearest`/`QuerySegment`(Start에서 가까운 순). 쿼리 범위는 최대 몬스터 반지름 + 프레임당 최대 이동량(`MaxPushPerFrame`)만큼 넓힌다. 살아 있는 몬스터만 반환. 콘은 `QueryCircle` + 호출처 각도 필터
  - [x] 호출처 전부 이전: `SelectTargetsInRadius`, `SelectNearestTarget`, `SelectTargetsBySweep`, `CullByCone`, `DEGameplayLibrary`(`ApplyAoEDamage`/`GetNearestTarget`/`GetRandomTargets`), AcidBolt, GoreGland, `DESimpleAOEBase`, `DESimpleProjectileBase`. C++에 남은 물리 쿼리 없음
  - [ ] 인게임 확인 후 몬스터 캡슐 `NoCollision`. 끄기 전에 BP에서 몬스터 콜리전/오버랩에 의존하는 곳이 없는지 확인 필요 (C++에서는 없음)
  - 판정 차이: 예전엔 3D 구 vs 캡슐, 지금은 XY 평면에서 원 vs 원(몬스터 반지름 `CachedRadius`). 그리드는 SpawnManager Tick에서 갱신되므로 이번 프레임에 스폰/재배치된 몬스터는 다음 프레임부터 잡힌다
- [ ] 렌더링을 몬스터별 SkeletalMesh에서 ISM + VAT로 (개발자가 ISM 진행 승인, 공부 목적 포함)
- [x] `ResolveOverlaps`의 프레임마다 생성되는 로컬 `TSet` 재사용 (`TouchedCells` 멤버 하나로 통합)
- [x] `ADEMonsterRepositionManager` 재배치 Z가 플레이어 Z 그대로였음. 몬스터 이동은 Z를 안 바꾸므로 몬스터 자신의 현재 Z를 유지

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
