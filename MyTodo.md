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

## 몬스터 VAT (ISM)

- [ ] 몬스터 하나로 VAT 굽기 (Bone 방식, 걷기 루프). 참고 예제는 엔진 플러그인 콘텐츠 `AnimToTexture/Characters/Mannequin`(`DA_BoneAnimation`, `BP_AnimToTexture`, `M_Body_BoneAnimation`). 흐름: SkeletalMesh에서 StaticMesh 만들기 → `AnimToTextureDataAsset` 생성(SkeletalMesh, StaticMesh, Mode=Bone, Animations에 걷기, AutoPlay 켬) → 굽기 → StaticMesh 머티리얼을 Bone 애니메이션 머티리얼(인스턴스)로. MCP(NiagaraToolsets와 같이 켠 AllToolsets)로 같이 해도 된다
- [ ] `DT_MonsterData` 그 몬스터 행의 `VATData`에 DataAsset 지정 (`VATAnimationIndex`는 걷기 순서, 보통 0)
- [ ] PIE: 그 몬스터가 걷는 애니메이션으로 보이는지, 방향/높이가 전 SkeletalMesh와 같은지, 죽으면 사라지는지. 로그에 `Monster ISM created for ...`가 한 번 뜨는지
- [ ] `VATData`가 없는 몬스터와 보스는 전처럼 보이는지

## 몬스터

- [ ] 접촉 공격: 이제 무적이 아니다. 몬스터에 붙으면 HP가 깎이는지 확인 (아래 "무적 해제와 난이도 곡선" 참고)
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
- [ ] 3개/5개 상자에서 같은 스킬이 두 번 나오면 카드가 Lv+1, Lv+2로 표시되고 닫은 뒤 둘 다 적용되는지. 만렙 1 전인 스킬은 한 상자에 한 번만 나오는지 (스킬을 1개만 갖고 Luck을 올려 5개 상자를 노리면 잘 보인다)
- [ ] 진화로 바뀐 연사 스킬(BurstInterval)의 옛 연사가 진화 순간 멈추는지
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
- [ ] 궤도 스킬(DanseMacabre)의 ExecutionType이 무엇인지 (`bCooldownAfterDuration = true`라 Duration으로 추정). Permanent면: 레벨업으로 개수가 늘 때 해골이 즉시 늘고 균등 간격으로 재배치되는지, 스탯이 바뀔 때 해골이 튀지 않는지
- [ ] 궤도 스킬 DT의 OptionValues에 `OrbitRadius`가 있으면 이제 그 반경으로 돈다 (전엔 스폰 위치에만 쓰이고 150으로 돌았음). 회전 속도는 `OrbitSpeed`(도/초, 음수면 시계)를 추가해서 조절. 범위/투사체 속도 스탯을 찍으면 반경/속도가 늘어나는지
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

## 투사체 판정 범위

- [ ] 투사체의 콜리전 구체가 이제 DT Radius 크기로 보인다 (판정도 원래 DT Radius였음). `show Collision`으로 Inferno를 쏴서 구체에 닿은 몬스터만 맞는지 확인
- [ ] 투사체 스킬마다 구체 크기가 나이아가라/메시와 맞는지 보고, 안 맞으면 `DESkillDataTable`의 Radius를 조정 (Inferno, DiabloInferno, BloodyKnife, AcidBolt 등). 생성자의 `InitSphereRadius`는 이제 의미 없다
- [ ] DT Radius가 0인 투사체 스킬이 있으면 몬스터 캡슐 반경으로만 맞는다 (전과 같음). 구체가 안 보이면 그 경우

## CharacterBase 정리 후 확인

- [ ] BP 컴파일: 캐릭터에서 `DebugAccessoryToEquip` 프로퍼티를 삭제했다 (BP에서 값만 넣었다면 문제없음)
- [ ] Project Settings > Input의 `BloodDrain` 액션 매핑은 이제 C++에서 바인딩하지 않는다 (디버그 메시지만 띄우던 것). 흡혈은 `ActiveSkill` 키. 필요 없으면 매핑 삭제
- [ ] 치트 키(DebugCheat, AnotherDebugCheat)와 콘솔 `ForceLevelUp`이 에디터/Development에서는 여전히 동작하는지
- [ ] 흡혈 시전 중 자동 스킬이 멈췄다가 끝나면 다시 도는지 (일시정지 플래그를 SkillManager 하나로 합침)

## 캐릭터 선택

- [ ] 로비에서 캐릭터 → 스테이지를 골라 입장했을 때 고른 캐릭터로 스폰되는지. 지금 DT_Characters 행은 `0`(Female Vampire)과 `1`이다. 행 `1`의 `CharacterClass`가 비어 있으면 Output Log에 `[Stage] Character 1 has no row or CharacterClass`가 뜨고 FemaleVampire로 나온다
- [ ] 스테이지 맵을 PIE로 바로 켰을 때 지금처럼 FemaleVampire로 나오는지
- [ ] 메뉴 위젯 BP(`UDEMainMenuWidget` 자식)에서 `CharacterDataTable` 프로퍼티를 삭제했다. 캐릭터 목록이 그대로 뜨는지 (이제 GameInstance가 `DT_Characters`를 직접 로드)
- [ ] 스테이지 GameMode BP가 `DefaultPawnClass`를 따로 지정했다면 그 값이 옵션 없을 때의 기본값이다
- [ ] 플레이어 사망 시 게임오버가 뜨는지: 무적을 해제했으니 몬스터에게 맞아 죽어본다. 결산 화면이 "게임 오버"로 뜨고 멈추면 OK
- [ ] 캐릭터 BP에 `CharacterData`를 지정하고 `LevelMilestones`에 항목을 넣어 레벨업 때 스탯이 오르는지. `MultiplierBonus`는 이제 "추가 %"다 (20% 증가 = 0.2). 기존 CharacterData 에셋에 1.2 같은 값을 넣어뒀다면 고쳐야 한다

## AOE

- [ ] Darkness/GreyDarkness: 범위 안 몬스터가 HitInterval마다 한꺼번에 맞는지, 처음 켰을 때 바로 한 번 맞는지 확인
- [ ] DanseMacabre(Orbit): 해골이 지나갈 때 몬스터가 맞는지, 같은 몬스터가 HitInterval보다 자주 맞지 않는지 확인

## SpatialGrid 쿼리 (몬스터 콜리전은 아직 켜져 있음)

스킬 판정이 이제 물리 씬 대신 그리드(`UDEMonsterQuerySubsystem`)를 쓴다. 판정은 XY 평면의 원 vs 원(몬스터 반지름은 DT `CapsuleRadius`)이다. 아래가 다 맞으면 몬스터 캡슐 콜리전을 끈다. 몬스터가 많은 곳(100마리 이상)에서 볼 것.

- [ ] 투사체 (`DESimpleProjectileBase`, Inferno/BloodyKnife 등): 날아가는 길에 닿은 몬스터가 맞는지, 관통 횟수만큼만 맞는지, 한 번 맞은 몬스터를 같은 투사체가 또 때리지 않는지
- [ ] AcidBolt 폭발 (`DEProjectile_AcidBolt`): 폭발 반경 안 몬스터가 다 맞는지. 동작 방식은 아래 "Behavior 스폰 액터"의 AcidBolt 항목 참고
- [ ] GoreGland 소환수 (`DESummon_GoreGland`): 소환수가 근처 몬스터를 찾아 공격하는지
- [ ] 장판/궤도 (`DESimpleAOEBase`): Darkness/GreyDarkness가 범위 안을 한꺼번에 때리는지, DanseMacabre 해골이 지나가며 맞히는지 (위 "AOE"와 같이 확인)
- [ ] 가장 가까운 적 조준 (`SelectNearestTarget`): 조준 스킬이 실제로 제일 가까운 몬스터 쪽으로 쏘는지, 검색 반경 밖에 있을 때 안 쏘는지
- [ ] 범위 타겟 (`SelectTargetsInRadius`): 반경 스킬이 반경 안만 맞히는지. `bTargetAllMonsters`를 켠 스킬이 있으면 화면 밖 몬스터까지 맞는지
- [ ] 부채꼴 (`CullByCone`, TalonOfCrow 등): 바라보는 방향의 부채꼴 안만 맞는지, 등 뒤 몬스터는 안 맞는지
- [ ] 직선/스윕 (`SelectTargetsBySweep`): 직선 위 몬스터가 가까운 순서로 맞는지
- [ ] 체감 판정 크기: 몬스터 가장자리를 스치는 공격이 예전과 비슷하게 맞는지. 크게 다르면 어떤 스킬, 어떤 몬스터인지 알려줘
- [ ] 방금 스폰된 몬스터와 재배치된 몬스터도 맞는지 (그리드는 다음 프레임부터 반영된다. 한 프레임 늦는 건 정상)
- [x] BP에서 몬스터 콜리전이나 오버랩에 의존하는 곳 없음 (개발자 확인, 2026-10-20). C++도 없음 (BloodDrain은 SpawnManager의 활성 몬스터 목록을 쓴다. 몬스터 이동은 sweep 없이 위치를 직접 지정하고, 플레이어 캡슐은 원래 Monster 채널과 Overlap이라 막히지 않는다)

## 스테이지 클리어

- [ ] `WBP_Result`에 TextBlock `Text_ResultTitle` 추가 (선택. 없으면 제목만 안 바뀐다). 문구는 WBP Class Defaults의 `ClearTitleText`/`GameOverTitleText`
- [ ] 스테이지 GameMode BP의 Class Defaults에서 `StageDuration`을 30 같은 짧은 값으로 바꿔 PIE: 30초에 멈추고 결과창이 클리어로 뜨는지, 시간이 0:30인지, 골드가 한 번만 정산되는지 확인 후 1800으로 되돌리기
- [ ] ESC 메뉴의 나가기는 지금처럼 게임오버로 뜨는지
- [ ] 웨이브 DT가 30분까지 채워져 있는지 (마지막 웨이브 이후 무엇이 나오는지)

## 무적 해제와 난이도 곡선

- [ ] 몬스터 여러 마리에 둘러싸였을 때 HP가 대략 0.2초에 한 번씩만 깎이는지 (데미지 텍스트 간격으로 확인 가능)
- [ ] 콘솔 `God`으로 무적이 켜지고 꺼지는지 (`God mode on`/`off` 메시지)
- [ ] 캐릭터 BP의 StatComponent에서 `Armor`, `Regeneration`을 잠깐 올려 보기: Armor 5면 데미지 5인 몬스터에게 1씩, Regeneration 2면 1초마다 2씩 차는지
- [ ] 캐릭터 BP StatComponent의 `InvulnerabilityTime` Base가 0.2인지 (BP가 예전 값을 저장해 두었으면 0일 수 있다. 0이면 무적시간 없음)
- [ ] 몬스터 DT의 `AttackDamage`가 지금 기준에 맞는지 (전에는 아무 의미 없었던 값)
- [ ] 스테이지 GameMode BP Class Defaults > Stage|Difficulty에서 `MonsterHPCurve`/`MonsterDamageCurve`에 키 넣기 (X 분, Y 배율. 예: 0분 1.0, 10분 3.0, 20분 8.0, 30분 15.0). 비워 두면 배율 1. 짧게 테스트하려면 0분 1, 1분 10 같은 커브로 1분 뒤 몬스터가 확 단단해지는지 확인
- [ ] BloodDrain은 최대 HP 비율 피해라 HP 배율이 올라도 같은 비율로 깎이는지

## 피격 연출

- [ ] 플레이어 캐릭터 머티리얼(BP 메시에 쓰는 것 전부)에 스칼라 파라미터 `HitFlash`(기본 0) 추가. 예: `Lerp(원래 색, 빨강, HitFlash)`를 Base Color나 Emissive에 연결. 이름을 바꾸려면 캐릭터 BP Class Defaults > Feedback > `HitFlashParameterName`
- [ ] 맞을 때 캐릭터가 0.12초 번쩍이는지, 둘러싸여 계속 맞으면 계속 켜져 있는지 (`HitFlashDuration`으로 조절)
- [ ] 플레이어 머리 위에 데미지 숫자가 안 뜨는지 (몬스터 숫자는 그대로)
- [ ] `UI_GameHUD`에 화면 가장자리 비네트 이미지(기본 숨김)와 페이드/맥동 애니메이션 추가. Graph에서 `Event On Low HP Changed` 구현: true면 켜고 재생, false면 끄기. 기준은 Class Defaults의 `LowHPThreshold`(0.3)
- [ ] HP 30% 아래로 떨어질 때 비네트가 켜지고, 회복해서 넘어가면 꺼지는지

## 캐릭터 플레이어 전용 설정 분리

- [ ] 메타 업그레이드(상점 HP/데미지/속도 등)가 여전히 적용되는지. 시작 HP가 늘어난 최대 HP로 가득 찬 상태인지 (전에는 최대 HP만 늘고 현재 HP는 100이었을 수 있음)
- [ ] 경험치 구슬, 골드, 상자 등 픽업을 여전히 주울 수 있는지 (`Player` 태그를 생성자에서 빙의 시점으로 옮김)
- [ ] 플레이어 머리 위 데미지 숫자가 여전히 안 뜨는지

## 몬스터 콜리전 끄기 후 확인

- [x] PIE에서 `show Collision`: 몬스터 캡슐이 안 보이고 스킬은 그대로 맞는지
- [x] 재측정 (Game 7.51ms, Todo 측정 ③): 에디터를 닫고 Standalone, `t.MaxFPS 0`, `r.VSync 0`, `TestPrototype` 300마리 `stat unit` + `stat game`. 비교 기준은 측정 ①(Game 12.80)과 ②(Game 9.64). 캡처는 새 이름으로 (예: `Docs/nocol_final_unit.png`)

## 상태이상

- [ ] Reference Viewer: `BP_StatusEffect_Venom_DoT`, `BP_StatusEffect_Venom_Damage`를 쓰는 스킬이 있는지 (에셋 문자열 검색으로는 없음)
- [ ] BloodyScarify: 폭발 범위가 전처럼 스킬 Radius를 따르는지, 폭발이 연쇄로 다른 몬스터를 죽여도 크래시가 없는지
- [ ] BloodDrain, GlareOfBasilisk 스턴이 전처럼 걸리고 풀리는지
- [ ] `BP_StatusEffect_BloodyScarifyExplosion`에 새 `ExplosionDamageTags`가 생겼다 (전엔 StatusTags가 데미지 태그로 들어갔다). 속성 태그(`Damage.Affinity.*` 등)를 붙일 거면 여기에. 지금 데미지 태그를 읽는 곳은 Execution/InstantKill/DoT뿐이라 비워둬도 동작은 같다
- [ ] `DT_MonsterData`에 `ImmuneStatusTags` 컬럼이 생겼다. 스턴 면역 몬스터를 만들려면 `Status.Debuff.Stun`처럼 효과 BP의 StatusTags와 같은 태그를 넣는다 (보스도 자동 면역은 아님. 광흡 면역은 따로 `bImmuneToBloodDrain`)

## 진영 (스킬 타겟 쿼리 변경 후)

- [ ] 모든 스킬이 전처럼 몬스터를 맞히는지 (특히 맵 전체 대상 스킬(`bTargetAllMonsters`), 발사체 관통 순서, 장판, GoreGland, AcidBolt 폭발, BloodyScarify 폭발)
- [ ] 플레이어 자기 스킬에 플레이어가 맞지 않는지
- [ ] Reference Viewer: `ADEGameMode_Stage::GetActiveMonsters`를 BP에서 쓰는지 (C++ 호출처는 없어짐)
- [ ] Reference Viewer: `DEItemBase`를 부모로 쓰는 BP가 있는지 (C++/에셋 문자열 검색으로는 없음). 없으면 삭제

## 성능 측정

- [ ] `r.RayTracing=False` 후 (에디터 재시작 필요, 셰이더 재컴파일): 측정 ③과 같은 조건(Standalone, 300마리, `stat unit`)으로 Draw/RHIT/GPU가 줄었는지. 화면이 달라진 곳이 없는지

- [ ] `TestStage001`/`TestBlank`에서 `NavMeshBoundsVolume`, `RecastNavMesh` 삭제 (내비 사용처 없음. `TestPrototype`은 완료, 미커밋)
- [ ] 캡슐 내비 제외 빌드(빌드 완료)로 `TestPrototype` 300마리 `stat unit` 재측정 (이전: Game 12.9ms, `Docs/unit.PNG`). 캡처는 새 이름으로 (예: `unit_nav.PNG`)

## 픽업

- [ ] 오래 플레이해도(누적 픽업 400개 이상) 드롭이 계속 나오는지 확인

## 풀

- [ ] 플레이 중 Output Log에 `[Pool] Duplicate return ignored: <액터 이름>`이 뜨는지 확인. 뜨면 액터 이름을 알려줘 (이중 반환하는 호출처 추적용. AcidBolt, 스킬 진화/취소 시점 위주로)

## 킬 집계/킬러 전달 후 확인

- [ ] 몬스터를 잡을 때마다 BloodDrain 게이지가 전처럼 차는지 확인 (이제 막타 친 플레이어에게 준다)
- [ ] 스킬 BP의 `LocalEffects`에 Execution을 단 게 있으면, 처형 킬에서 Output Log에 `SourceObject null` Warning이 더는 안 뜨는지 확인
- [ ] BloodDrain 시전 중 범위 안 몬스터가 실제로 멈추는지, 바깥 몬스터가 링을 밀고 들어오지 않는지 확인. 몬스터가 많을 때 벽 앞에서 떨리거나 겹쳐 보이면 알려줘
- [ ] 석화/스턴 스킬(GlareOfBasilisk 등)에 걸린 몬스터도 이제 다른 몬스터를 막는 벽이 된다. 의도와 맞는지 확인
- [ ] BloodDrain 시전 중에 GlareOfBasilisk로 같은 몬스터를 석화: 석화 시간이 짧아도 배리어 몬스터가 BloodDrain 스턴 시간까지 멈춰 있는지. 반대로 석화가 더 길면 그만큼 멈춰 있는지
- [ ] 스턴 걸린 채 죽은 몬스터가 풀에서 다시 나올 때 정상적으로 움직이는지. Output Log에 `RemoveMoveBlock without a matching AddMoveBlock`이 뜨면 알려줘
- [ ] `NS_BloodDrainCustom`에 float User Parameter `DrainDuration`(시전 시간, 초) 추가. 흐름이 플레이어에게 모이는 시점을 이 값에 맞추면 마무리 데미지와 맞는다. 위치 배열은 기존 `MonsterPosi`, 플레이어는 `PlayerPosition`
- [ ] 몬스터 DT에서 보스 행의 `MonsterRank`가 Boss인지 확인. 이제 DT 값이 몬스터에 들어가고, Boss면 광흡 대상에서 빠진다
- [ ] 몬스터 DT에서 광흡 면역으로 할 엘리트 행에 `bImmuneToBloodDrain` 체크 (새 컬럼, 기본 false)
- [ ] BloodDrain BP(있으면)에서 `DrainDamagePercent` 조정 (기본 0.3 = 최대 HP의 30%)
- [ ] 에디터를 열 때 BP 컴파일 에러가 없는지: `UDEHealthComponent::InstantKill`(BlueprintCallable)과 몬스터/캐릭터 `TakeDamage` 오버라이드를 삭제했다. 에셋 문자열 검색으로는 참조가 없었다

## SpawnManager 정리 후 확인

- [ ] `TestPrototype`/`TestStage001`/`TestBlank` 맵을 열어 SpawnManager 인스턴스가 정상 로드되는지 확인하고 다시 저장. 루트였던 `MonsterHISM`이 빠지고 `RootScene`이 루트가 되면서 액터 위치가 원점으로 갈 수 있다 (코드는 SpawnManager 위치를 안 쓰므로 동작엔 영향 없음)

## 템플릿 삭제 후 확인

- [ ] 에디터를 열어 깨진 참조 경고가 없는지 확인. `L_Lobby`, `Lobby`, `TestMap`에는 GameMode Override가 없는 것으로 보여서, 이제 기본값인 `BP_DEGameMode_Stage`로 뜬다. `L_Lobby`(결과 화면에서 돌아가는 로비)는 World Settings에서 Lobby GameMode를 지정해야 한다
- [ ] 문제없으면 `../DarkEdenSurvivors_DeletedBackup_20261019/` 백업 폴더 삭제
