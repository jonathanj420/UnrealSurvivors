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
- [x] 일시정지와 입력 모드 관리를 한 곳으로: `ADEPlayerController`가 멈춘 이유 집합(`EDEPauseReason`: LevelUp/Chest/PauseMenu/GameOver)을 갖고, `SetPause`/입력 모드/커서는 `ApplyPauseState` 한 곳에서만 바꾼다. 이유가 하나라도 남아 있으면 멈춘 상태 유지. 멈춘 동안 입력 모드는 GameAndUI (UIOnly는 ESC 바인딩까지 막는다). ESC는 다른 이유로 멈춰 있으면 무시. `GameOver`도 이 경로를 쓰고 `GetFirstPlayerController`로 입력 모드를 바꾸던 코드 삭제. `ResumeGame` 삭제. 멀티플레이에서 이유별로 월드를 멈출지는 `ApplyPauseState`에서 정하면 된다
  - BloodDrain의 `SetAutoSkillsPaused`는 월드 일시정지가 아니라 게임플레이 규칙이라 대상 아님
- [ ] 스턴 2개 → StatusEffect 하나로
- [x] 레벨업 큐: `UDELevelUpManagerComponent`가 대기 레벨업 수와 현재 선택지를 갖는다. `EnqueueLevelUp` → 선택지 생성 → `OnLevelUpChoicesReady` → 컨트롤러가 위젯 하나를 재사용해 표시 → 슬롯 클릭 → `ApplyChoice`(현재 선택지에 있는 것만 적용) → 다음 선택지 또는 `OnLevelUpClosed` → 컨트롤러가 닫는다. 선택지가 0개인 레벨업은 건너뛴다. 위젯은 더 이상 선택지를 만들거나 적용하지 않는다
  - [x] 상자 보상도 매니저로: `EnqueueChest` → 보상 생성 → `OnChestRewardsReady` → 컨트롤러가 표시 → 닫기 버튼 → `ClaimChestRewards` → 다음 상자 또는 `OnChestClosed`. 같은 프레임에 주운 상자가 덮어써져 사라지던 문제 해결. `ADEPickup_Chest`의 `GetPlayerController(0)` 제거
  - [x] 레벨업과 상자는 한 번에 하나만 띄운다 (겹치면 레벨업 먼저). 창이 겹쳐 보이던 문제와, 상자 보상이 레벨업 적용 전 상태로 만들어지던 문제 해결
- [ ] 킬 처리를 `FDEDamageResult` 기반으로 일원화 (킬 카운터 중복 해소)
- [x] `EvolveSkill`이 `EndSkill` 대신 `CancelSkill`을 쓰도록. `CancelSkill`은 `RegisterSkillActor`로 등록된 액터(Orbit, SpawnAOE 등)도 회수한다. 날아가는 투사체는 추적하지 않으므로 수명이 다할 때까지 남는다
- [ ] 레거시 스킬을 BP 조립 방식으로 이전

## 스킬 습득/레벨업/진화 (2026-10-20 리뷰)

- [x] 발동/Behavior마다 찍히던 로그 삭제 (`Activate`, `ExecutePipeline`, `SpawnAOE`, `PlayEffect` 빔). `SetSkillData`의 null 역참조 수정
- [x] 상자 `GetFallbackChoice()`의 nullptr이 보상 배열에 들어가던 문제
- [x] "올릴 수 있나" 판정 일원화: `UDESkillManagerComponent::CanLevelUp`을 레벨업 카드(`GetRandomSkillRow`), 상자(`GetRandomUpgradableChoice`), `LevelUpSkill`(이제 bool 반환)이 같이 쓴다. 슬롯이 꽉 차면 새 스킬 카드가 안 나온다. 최대 레벨은 DT 행 기준으로 결정 (개발자, 2026-10-20). `FDESkillRow::MaxLevel`은 판정에 안 쓴다
  - 동작 변경: 이미 얻은 진화 결과 스킬도 DT에 다음 레벨 행이 있으면 레벨업 카드에 나온다 (전에는 상자에서만 올라갔다)
  - `FDESkillRow::MaxLevel` 컬럼 삭제 여부는 BP에서 쓰는지 Reference Viewer로 확인 후 결정
- [x] 상자에서 진화 재료가 업그레이드로 같이 뽑혀 Lv1로 다시 생기던 문제: 진화 카드의 `ConsumedSkillIDs`를 업그레이드 후보에서 뺀다
- [ ] 상자에서 같은 스킬이 업그레이드로 두 번 뽑히면, 남은 레벨이 1개일 때 두 번째는 적용 실패한다 (뽑을 때 예정된 레벨업을 반영하지 않음)
- [x] 상태 일원화: `SkillLevels`, `UDESkillBase::CurrentLevel`(쓰기만 함) 삭제. 레벨은 `FActiveSkill::CurrentLevel` 하나. Inventory의 스킬 목록과 `MaxSkillSlots`를 `UDESkillManagerComponent::SkillSlotOrder`/`MaxSkillSlots`로 이동 (개발자 결정, 2026-10-20). HUD 스킬 위젯은 SkillManager의 `OnSkillUpdated`만 본다. 진화 결과는 재료 중 가장 앞 슬롯에 들어간다. 결과 스킬 데이터가 없으면 재료를 지우기 전에 진화를 취소한다
- [ ] `UDEInventoryComponent`에는 장신구 ID 목록만 남았는데, `UDEAccessoryComponent`의 장착 맵과 겹친다. 쓰는 곳은 `CheckEvolution`의 `HasAccessory` 하나. AccessoryComponent로 합치고 Inventory 삭제 검토
- [x] 레벨업 시 쿨다운 0 리셋이 진행 중인 시전을 끊던 문제: 레벨업하면 바로 재발동하되, 이전 시전(Delay로 멈춘 파이프라인, `FireProjectile` 연사)이 끝난 뒤에 발동한다 (개발자 결정 (b), 2026-10-20). `UDEAutoSkillBase::IsCastInProgress` + `UDESkillBehavior::IsInProgress`. 쿨다운이 시전 시간보다 짧을 때도 같은 이유로 끊기던 것이 같이 해결된다. 자체 타이머를 쓰는 Behavior를 새로 만들면 `IsInProgress`를 오버라이드할 것
- [ ] `FireProjectile` 연사 타이머는 `EndBehavior`에서 정리되지 않는다. 진화로 `CancelSkill`된 스킬의 남은 연사가 끝까지 나간다
- [ ] 죽은 코드: `GetRandomSkillChoices`, `GetUpgradeableSkills`, `ApplyCharacterDamageMultiplier`, `DeltaCheck`, `TotalReducedCooldownAmount`, `AccessoryWeight`, `bCooldownAfterDuration`(set만 하고 안 읽음)
- [ ] 스킬 DT를 생성자의 하드코딩 경로로 로드해서 `SkillRowMap`을 만든다. `SkillRowTable`은 EditAnywhere라 BP에서 바꿔도 맵에 반영되지 않는다

## Behavior (2026-10-20 리뷰)

- [x] 스폰 액터 기록을 두 가지로 통일: 키당 하나인 오라는 `OwnedSkillActorMap` + `FindKeyedSkillActor`(`IsActiveFor` 확인), 여러 개 스폰되는 액터(SpawnAOE의 키 없는 장판, SpawnOrbit, SpawnSummon, SpawnOnMove)는 `RegisterSkillActor`로 배열에 등록하고 등록할 때 죽은 항목을 정리. `FDESkillContext::SpawnedAOEs` 삭제. `CancelSkill`은 두 곳만 거둔다
  - 해결: Permanent 소환 스킬 레벨업 때 풀에 반환된 소환수가 풀 안에서 되살아나 다른 스킬과 같은 액터를 쓰던 문제, 죽은 소환수를 살아 있다고 세서 보충 소환이 안 되던 문제
  - 해결: SpawnOnMove/SpawnSummon이 스폰마다 GUID FName 키를 만들어 맵과 FName 테이블이 계속 커지던 문제
  - 동작 변경: `CancelSkill`(진화)이 마지막 발동분뿐 아니라 살아 있는 키 없는 장판 전부를 거둔다
  - SpawnOrbit의 풀 액터 `Destroy()` 제거
- [ ] Orbit은 `OnContextRefreshed`가 없다. Orbit 스킬이 Permanent면 레벨업해도 개수와 스탯이 안 바뀐다 (ExecutionType은 DT 확인 필요)
- [x] `CullByCone::ConeAngle`이 이제 읽힌다 (DT `"Angle"`이 우선, 없으면 ConeAngle). 기본값을 120에서 60으로 바꿔서 손대지 않은 BP는 전과 같은 60도
- [x] `InstantDamage::HitCount`/`HitInterval` 구현 (개발자 결정, 2026-10-20). 간격 0이면 한 번에, 0보다 크면 첫 타격 후 남은 타격을 타이머로. 파이프라인은 기다리지 않는다. `IsInProgress`로 다음 발동을 미루고 `EndBehavior`에서 정리. 타격으로 죽은 타겟은 바로 빼서 풀에서 재사용된 몬스터를 때리지 않는다 (다른 스킬이 죽인 뒤 간격 안에 재사용되면 막지 못함). 뒤 Behavior가 보는 `Context.Targets`는 바꾸지 않는다
  - 체력 컴포넌트 없는 대상의 `UGameplayStatics::ApplyDamage` 폴백 삭제 (타겟은 몬스터 쿼리로만 들어온다)
- [x] 안 쓰는 프로퍼티 삭제: `PlayEffect::TargetType`(레거시 4개 스킬은 같은 의미의 `NiagaraTargetType`으로), `FireProjectileRadial::ProjectileClass`, `SelectNearestTarget::EnemyTag`, `SelectTargetsInRadius::VectorKeyName`/`bIgnoreInstigator`. `ApplyStatusEffect`의 빈 TODO 루프. `NiagaraTargetType`/`SoundTargetType` 기본값을 명시 (기존과 같은 Instigator)
- [ ] 레거시 `DESkill_AcidBolt::InitBehaviors`의 `FireProjectileRadial`은 `ProjectileClasses`를 채우지 않아서 한 발도 안 쏜다 (전에도 `ProjectileClass`만 넣어서 같았다). AcidBolt가 BP에서 Behavior를 따로 조립해 동작 중이면 C++ 쪽은 BP 이전 때 삭제
- [ ] 값 우선순위 규칙 통일 (개발자 결정 대기): Behavior마다 프로퍼티/Context/CustomValues 중 무엇이 이기는지, -1의 의미가 다르다. `SelectTargetsInRadius`의 맵 전체 검색은 `GetGameMode` 경유

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
