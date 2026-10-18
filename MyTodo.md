# MyTodo

개발자가 에디터에서 직접 해야 하는 작업 (BP, DT, 레벨, Reference Viewer 확인, 인게임 테스트). 코드 작업은 `Todo.md`.

## 골드

- [ ] `Content/DarkEden/Blueprint/PickUps/`에 `BP_DEPickup_Gold` 생성 (부모 `DEPickup_Gold`), 메시 지정 (후보: `Book/.../SM_Treasure_Coin`)
- [ ] `DT_MonsterData`의 몬스터 `DropTable`에 항목 추가: DropClass `BP_DEPickup_Gold`, DropChance(예: 30), ItemValue(드롭 1개당 골드)
- [ ] PIE에서 골드를 주웠을 때 GameMode Details의 `EarnedGold`가 오르는지 확인
- [ ] 콘솔 `ke * GameOver`로 정산 확인 (이 명령이 동작하는지는 미검증). 로비 상점에서 총 골드가 늘었는지 확인

## 죽은 코드 삭제 전 확인 (Reference Viewer)

- [ ] Test* 클래스, 템플릿 GameMode/PC, `DEMonsterDataAsset`, `DESimpleMeleeBase`, `DEBloodyNail` 액터가 BP/레벨에서 참조되는지
