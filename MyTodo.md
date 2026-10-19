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

## 성능 측정

- [ ] `TestPrototype`/`TestStage001`/`TestBlank`에서 `NavMeshBoundsVolume`, `RecastNavMesh` 삭제 (내비 사용처 없음)
- [ ] 위 삭제 + 캡슐 내비 제외 빌드로 300마리 `stat unit` 재측정 (이전: Game 12.9ms)

## 픽업

- [ ] 오래 플레이해도(누적 픽업 400개 이상) 드롭이 계속 나오는지 확인

## 풀

- [ ] 플레이 중 Output Log에 `[Pool] Duplicate return ignored: <액터 이름>`이 뜨는지 확인. 뜨면 액터 이름을 알려줘 (이중 반환하는 호출처 추적용. AcidBolt, 스킬 진화/취소 시점 위주로)

## SpawnManager 정리 후 확인

- [ ] `TestPrototype`/`TestStage001`/`TestBlank` 맵을 열어 SpawnManager 인스턴스가 정상 로드되는지 확인하고 다시 저장. 루트였던 `MonsterHISM`이 빠지고 `RootScene`이 루트가 되면서 액터 위치가 원점으로 갈 수 있다 (코드는 SpawnManager 위치를 안 쓰므로 동작엔 영향 없음)

## 템플릿 삭제 후 확인

- [ ] 에디터를 열어 깨진 참조 경고가 없는지 확인. `L_Lobby`, `Lobby`, `TestMap`에는 GameMode Override가 없는 것으로 보여서, 이제 기본값인 `BP_DEGameMode_Stage`로 뜬다. `L_Lobby`(결과 화면에서 돌아가는 로비)는 World Settings에서 Lobby GameMode를 지정해야 한다
- [ ] 문제없으면 `../DarkEdenSurvivors_DeletedBackup_20261019/` 백업 폴더 삭제
