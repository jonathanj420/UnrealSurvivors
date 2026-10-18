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

## 픽업

- [ ] 오래 플레이해도(누적 픽업 400개 이상) 드롭이 계속 나오는지 확인

## 풀

- [ ] 플레이 중 Output Log에 `[Pool] Duplicate return ignored: <액터 이름>`이 뜨는지 확인. 뜨면 액터 이름을 알려줘 (이중 반환하는 호출처 추적용. AcidBolt, 스킬 진화/취소 시점 위주로)

## 죽은 코드 삭제 전 확인 (Reference Viewer)

- [ ] Test* 클래스, 템플릿 GameMode/PC, `DEMonsterDataAsset`, `DESimpleMeleeBase`, `DEBloodyNail` 액터가 BP/레벨에서 참조되는지
