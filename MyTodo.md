# MyTodo

개발자가 에디터에서 직접 해야 하는 작업 (BP, DT, 레벨, Reference Viewer 확인, 인게임 테스트). 코드 작업은 `Todo.md`.

## 골드

- [ ] `Content/DarkEden/Blueprint/PickUps/`에 `BP_DEPickup_Gold` 생성 (부모 `DEPickup_Gold`), 메시 지정 (후보: `Book/.../SM_Treasure_Coin`)
- [ ] `DT_MonsterData`의 몬스터 `DropTable`에 항목 추가: DropClass `BP_DEPickup_Gold`, DropChance(예: 30), ItemValue(드롭 1개당 골드)
- [ ] PIE에서 골드를 주웠을 때 GameMode Details의 `EarnedGold`가 오르는지 확인
- [ ] 콘솔 `ke * GameOver`로 정산 확인 (이 명령이 동작하는지는 미검증). 로비 상점에서 총 골드가 늘었는지 확인

## 스폰/웨이브

- [ ] 몬스터가 플레이어 주변 테두리 전체(상하좌우 포함)에서 나오는지 확인
- [ ] Timed/Infinite 웨이브에서 빠르게 잡아도 MinimumCount 수가 유지되는지, 웨이브 시작 시 MinimumCount만큼 한 번에 쏟아지는지 확인
- [ ] `DT_StageWave` 각 행의 `MinimumCount`가 이 의미(최소 유지 수)에 맞는 값인지 재검토. 예전엔 시작 버스트 수량으로만 쓰였다

## 보스

- [ ] 보스를 잡았을 때 드롭, 흡혈 게이지, 사라짐(풀 반환)이 정상인지 확인
- [ ] 보스가 땅에 박히지 않고 제 높이로 나오는지 확인 (이전엔 높이 보정 없이 Z=0)

## 스탯

- [ ] 에디터에서 `DEAccessory_Buman`의 StatModifiers Multiplier가 0.1, 나머지 액세서리는 0으로 보이는지 확인 (`FDEStatModifier::Multiplier` 기본값을 1에서 0으로 바꿈)
- [ ] 메타 업그레이드 Amount를 1레벨 이상 찍은 상태에서 발사체가 +1개만 늘어나는지 확인 (전에는 ×2)

## 몬스터

- [ ] 접촉 공격: 무적이라 화면엔 안 보인다. `ADECharacterBase::TakeDamage`에 브레이크포인트를 걸고 몬스터가 붙었을 때 걸리는지 확인 (전에는 오버랩 이벤트가 꺼져 있어서 한 번도 안 불렸다)
- [ ] `DT_MonsterData`에서 CapsuleRadius가 42가 아닌 몬스터가 스킬에 맞는 범위가 체감상 달라졌는지 확인 (전에는 물리 캡슐이 항상 기본 42/96이었다)
- [ ] MinimumCount 버스트 때 프레임 튐이 줄었는지 확인
- [ ] 멀리 떨어진 몬스터가 플레이어 근처로 재배치될 때 뜨거나 박히지 않는지 확인 (특히 CapsuleHalfHeight가 96이 아닌 몬스터)
- [ ] Standalone(`-log`)에서 몬스터 메시가 보이는지, 로그에 `Monster assets preloaded in N.NNs`가 뜨고 그 뒤에 첫 웨이브가 나오는지 확인

## 스킬 진화

- [ ] Orbit 스킬(DanseMacabre 등)이나 장판, 소환수 스킬을 재료로 진화시킨 뒤, 옛 스킬의 액터가 사라지는지 확인 (`CancelSkill`로 바꿈. 치트 키 `MyAnotherDebugCheat`로 진화 가능)
- [ ] 진화 직후 다른 스킬의 장판이나 오브가 같이 사라지지 않는지 확인 (풀 재사용 소유권 체크)
- [ ] HUD 스킬 슬롯에서 진화 결과가 재료가 있던 자리에 뜨는지 확인 (재료가 여러 개면 가장 앞 재료 자리)
- [ ] 스킬 획득과 레벨업 때 HUD 스킬 슬롯이 갱신되는지 확인 (갱신 신호를 Inventory에서 SkillManager로 바꿈)

## 상자

- [ ] 상자 하나: 열림 연출 → 클릭 → 보상 표시 → 닫기 → 보상 적용, 게임 재개
- [ ] 상자 두 개를 붙여 놓고 한 번에 줍기: 첫 상자를 닫으면 두 번째 상자가 열림 연출부터 다시 나오고 (닫기 버튼은 숨겨진 상태로), 마지막 상자를 닫은 뒤에만 재개되는지
- [ ] `WBP_ChestWidget`이 BP에서 `ApplyAllRewardsAndClose` 뒤에 직접 `RemoveFromParent`나 재개를 하고 있지 않은지 확인 (이제 컨트롤러가 한다)

## 일시정지

- [ ] BP 컴파일 확인: `ADEPlayerController::ResumeGame`을 삭제했다. WBP 등에서 부르고 있으면 컴파일 에러가 난다 (C++ 호출처는 없었음)
- [ ] ESC 메뉴: 열면 멈추고 커서가 보이며, 닫으면(ESC/계속하기) 재개되는지
- [ ] 레벨업 창, 상자 창, 결산 화면에서 ESC를 눌러도 아무 일 없는지
- [ ] ESC 메뉴의 나가기: 결산 화면이 뜨고 게임이 멈춘 채로 유지되는지
- [ ] 레벨업과 상자가 동시에 생기게 하기 (경험치 구슬과 상자를 붙여 놓고 줍기): 레벨업 창이 먼저 뜨고, 다 고른 뒤 상자 창이 뜨고, 상자를 닫은 뒤에만 재개되는지
- [ ] 레벨업/상자 창에서 클릭이 정상인지 (입력 모드를 UIOnly에서 GameAndUI로 바꿈)

## 스킬 레벨업

- [ ] 스킬 슬롯 6칸을 다 채운 뒤 레벨업 카드에 새 스킬이 안 나오는지 확인
- [ ] DT 행이 9개보다 적은 스킬을 최대 레벨까지 올린 뒤, 상자에서 그 스킬 업그레이드가 안 나오는지 확인
- [ ] `WBP_LevelUpWidget`과 그 `SlotWidgetClass`에 지정된 슬롯 BP가 경고 없이 컴파일되는지 확인. 슬롯의 `OnSlotSelected`에 선택한 Choice 파라미터가 추가됐다. BP에서 이 이벤트를 바인딩했다면 다시 연결해야 한다
- [ ] 레벨업 한 번: 카드 3장 → 선택 → 창이 닫히고 게임 재개
- [ ] 한 번에 여러 레벨 오르기 (경험치를 크게 주거나 `ForceLevelUp`을 일시정지 전에 여러 번): 창이 하나만 뜨고, 고를 때마다 카드가 새로 바뀌고, 마지막 선택 후에만 재개되는지. 두 번째 카드가 첫 선택을 반영하는지 (예: 방금 Lv2로 올린 스킬이 Lv3 카드로 나옴)
- [ ] 연사(BurstInterval) 스킬이나 Delay가 들어간 스킬을 연사/시전 도중에 레벨업: 진행 중인 발사가 끝까지 나가고, 끝나자마자 다음 발동이 나오는지 확인
- [ ] Reference Viewer로 `FDESkillRow::MaxLevel`(DESkillRowTable의 MaxLevel 컬럼)을 BP 위젯 등에서 쓰는지 확인. 안 쓰면 삭제 가능

## Behavior 스폰 액터

- [ ] 소환 스킬(GoreGland 등): 소환수가 수명이 다해 사라진 뒤 레벨업하면 빈자리만큼 새로 소환되는지, 사라진 소환수가 그 자리에서 다시 나타나지 않는지
- [ ] 마늘형 오라(SpawnAOE, Key 있음): 레벨업 때 오라가 두 개로 늘지 않고 크기와 데미지만 바뀌는지
- [ ] 이동 장판(SpawnOnMove)과 궤도 스킬을 재료로 진화: 깔려 있던 장판과 궤도가 진화 순간 사라지는지
- [ ] `PlayUI`로 띄우는 위젯 BP가 스스로 `RemoveFromParent`하는지 (C++은 띄우기만 한다)
- [ ] `FireProjectile`이 `FilterTargets`로 여러 타겟을 골라도 첫 타겟 방향으로만 쏘는 게 의도인지
- [ ] `CullByCone`을 쓰는 BP 스킬(TalonOfCrow 등)에서 `ConeAngle`을 60 말고 다른 값으로 넣어둔 게 있는지. 이제 그 값이 실제로 적용된다 (DT에 Angle이 없을 때)
- [ ] `InstantDamage`에서 `HitCount`/`HitInterval`을 BP에 넣어둔 스킬이 있는지. 전엔 무시됐는데 이제 그만큼 때린다
- [ ] HitCount 3, HitInterval 0.2 정도로 하나 만들어서: 3번 맞는지, 첫 타격에 죽은 몬스터 자리에 새로 나온 몬스터가 맞지 않는지
- [ ] BP 컴파일: `PlayEffect::TargetType`, `FireProjectileRadial::ProjectileClass`, `SelectNearestTarget::EnemyTag`, `SelectTargetsInRadius::VectorKeyName`/`bIgnoreInstigator`를 삭제했다 (BP 그래프에서 읽을 수 없는 EditAnywhere 전용이라 에러는 안 날 것)
- [ ] **값 우선순위 규칙 변경 확인** (프로퍼티 -1이면 DT, 0 이상이면 덮어씀). 기본값을 -1로 바꿔서 손대지 않은 BP는 대체로 그대로지만, 아래는 동작이 바뀔 수 있다:
  - `SelectTargetsInRadius`에서 **Radius를 -1로 넣어 맵 전체를 노린 BP 스킬**: 이제 `bTargetAllMonsters`를 체크해야 한다 (안 하면 DT 반경으로 검색). 레거시 C++ BloodyScarify/BloodySpear는 이전함
  - `SelectTargetsInRadius`에서 Radius를 직접 넣어둔 BP와 레거시 DeathSentence/Executioner(800): 전엔 무시되고 DT Radius를 썼는데, 이제 그 값으로 검색한다
  - `CullByCone` 스킬: 전엔 DT Radius가 0이면 150으로 대체됐는데, 이제 0이면 아무도 안 맞는다. 해당 스킬 DT의 Radius 확인
  - `SelectNearestTarget`의 DefaultSearchRadius를 넣어둔 BP와 레거시 AcidBolt(800)/DeadlyGround/GoreGland(1000): 전엔 DT OptionValues `"Radius"`가 있으면 그게 이겼는데, 이제 프로퍼티가 이긴다
  - `ApplyStatusEffect`의 Interval을 넣어둔 BP와 레거시 BloodyScarify(0): 전엔 DT `"Interval"`이 있으면 그게 이겼는데, 이제 프로퍼티가 이긴다
- [ ] AcidBolt가 지금 어떻게 동작하는지: 스킬 클래스가 C++ `DESkill_AcidBolt`인지 BP인지, BP라면 `FireProjectileRadial`의 `ProjectileClasses`를 채웠는지

## AOE

- [ ] Darkness/GreyDarkness: 범위 안 몬스터가 HitInterval마다 한꺼번에 맞는지, 처음 켰을 때 바로 한 번 맞는지 확인
- [ ] DanseMacabre(Orbit): 해골이 지나갈 때 몬스터가 맞는지, 같은 몬스터가 HitInterval보다 자주 맞지 않는지 확인

## SpatialGrid 쿼리 (몬스터 콜리전은 아직 켜져 있음)

- [ ] 스킬 전부가 예전처럼 맞는지 확인: 투사체(관통 횟수대로 가까운 적부터), AcidBolt 폭발, GoreGland, Darkness/DanseMacabre, 범위/부채꼴/스윕 스킬, 가장 가까운 적 타겟팅
- [ ] BP에서 몬스터 콜리전이나 오버랩에 의존하는 곳이 있는지 확인 (BP 스킬, 레벨 BP 등). 없으면 몬스터 캡슐 콜리전을 끈다

## 성능 측정

- [ ] `TestStage001`/`TestBlank`에서 `NavMeshBoundsVolume`, `RecastNavMesh` 삭제 (내비 사용처 없음. `TestPrototype`은 완료, 미커밋)
- [ ] 캡슐 내비 제외 빌드(빌드 완료)로 `TestPrototype` 300마리 `stat unit` 재측정 (이전: Game 12.9ms, `Docs/unit.PNG`). 캡처는 새 이름으로 (예: `unit_nav.PNG`)

## 픽업

- [ ] 오래 플레이해도(누적 픽업 400개 이상) 드롭이 계속 나오는지 확인

## 풀

- [ ] 플레이 중 Output Log에 `[Pool] Duplicate return ignored: <액터 이름>`이 뜨는지 확인. 뜨면 액터 이름을 알려줘 (이중 반환하는 호출처 추적용. AcidBolt, 스킬 진화/취소 시점 위주로)

## SpawnManager 정리 후 확인

- [ ] `TestPrototype`/`TestStage001`/`TestBlank` 맵을 열어 SpawnManager 인스턴스가 정상 로드되는지 확인하고 다시 저장. 루트였던 `MonsterHISM`이 빠지고 `RootScene`이 루트가 되면서 액터 위치가 원점으로 갈 수 있다 (코드는 SpawnManager 위치를 안 쓰므로 동작엔 영향 없음)

## 템플릿 삭제 후 확인

- [ ] 에디터를 열어 깨진 참조 경고가 없는지 확인. `L_Lobby`, `Lobby`, `TestMap`에는 GameMode Override가 없는 것으로 보여서, 이제 기본값인 `BP_DEGameMode_Stage`로 뜬다. `L_Lobby`(결과 화면에서 돌아가는 로비)는 World Settings에서 Lobby GameMode를 지정해야 한다
- [ ] 문제없으면 `../DarkEdenSurvivors_DeletedBackup_20261019/` 백업 폴더 삭제
