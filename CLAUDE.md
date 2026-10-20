# DarkEdenSurvivors

UE 5.7 / C++로 만드는 *Vampire Survivors*류 액션 서바이벌 게임이고, *DarkEden*을 테마로 한다. 포트폴리오가 목표이고, 가능하면 출시까지 간다.

- **GAS는 쓰지 않는다.** 명시적으로 요청받지 않으면 도입하지 않는다.
- **현재 싱글플레이, 멀티플레이는 향후 고려 중.** 리플리케이션 코드는 아직 없다. 아래 "멀티플레이 대비"를 따른다.
- **배경:** 원래 Gemini 웹 채팅으로 프로덕션 수준 구조를 배우면서 작성했다. 그래서 같은 기능이 여러 벌 있거나, 연결이 빠졌거나, 마이그레이션이 반쯤만 된 잔재가 많다. 개발자는 전체 플로우를 설명할 수 있고, **이 이해를 유지하는 것도 목표다.**
- 배경 문서: `Docs/DarkEdenSurvivors_Project_Context_v2.md`. 코드와 다르면 코드가 기준이다.

## 작업 원칙

- **바꾸기 전에 읽는다.** 관련 클래스와 호출처, 데이터 흐름, 소유권과 수명을 확인한 다음 문제가 실제인지 판단한다.
- **구조 결정은 개발자가 한다.** 선택지와 트레이드오프를 제시하고, 개발자의 제안도 기술적으로 평가한다. 무조건 동의하지도, 설명 없이 갈아엎지도 않는다.
- **설명은 중요한 결정에만 한다.** 아키텍처, 수명, 정확성, 확장성, 네트워킹에 영향을 주는 선택만 이유와 흐름을 짧게 설명한다. 사소한 줄까지 설명하지 않는다.
- **복잡함을 위한 복잡함은 피한다.** "AAA 스타일"이 최대 추상화를 뜻하지 않는다. 실제 문제를 푸는 구조를 고른다.
- **검증한다.** 빌드하고 로그를 확인한다. 확인한 것과 추정한 것을 구분해서 말한다.
- **변경은 작게, 커밋 단위로 한다.**

## 시스템 맵

| 영역 | 핵심 클래스 |
|---|---|
| 게임 플로우 | `UDEGameInstance`(세이브, 골드, 메타 업그레이드, 몬스터 DT 캐시, 캐릭터 DT), 캐릭터 선택은 메뉴 → `OpenLevel` 옵션 `Character=<id>` → `ADEGameMode_Stage::InitNewPlayer` → `GetDefaultPawnClassForController`, `ADEGameMode_Lobby` + `ADEPlayerController_Lobby` + `UDEMainMenuWidget`, `ADEGameMode_Stage`(ElapsedTime, GameOver), `ADEPlayerController`(HUD, 레벨업/상자 창 표시, 일시정지는 `EDEPauseReason` 집합으로 `ApplyPauseState` 한 곳에서) |
| 캐릭터 | `ADECharacterBase`(→ `ADEFemaleVampire`), `UDEStatComponent`, `UDEHealthComponent`, `UDECombatComponent`, `UDEStatusEffectComponent`, `UDEAccessoryComponent`, `UDEProgressionComponent`, `UDELevelUpManagerComponent`(레벨업과 상자 큐, 선택지 생성과 적용. 한 번에 하나씩, 레벨업 먼저) |
| 스탯 | `FGameplayStat` = `(Base + ΣAdditive) × (1 + ΣMultiplier)`. Multiplier는 "추가 %"다. 단, `FDEStatModifier::Multiplier`의 기본값이 1.0이라 버그가 있다. |
| 데미지 | `FDEDamageRequest` → `UDEGameplayLibrary::ApplyCombatDamage` → `UDEHealthComponent::ProcessDamage` → `FDEDamageResult` → 공격자 `UDECombatComponent::HandleDamageDealt`(OnHit/OnKill) |
| 스킬 | `UDESkillManagerComponent`(획득, 레벨, 쿨다운, 진화; `DESkillDataTable` 레벨별 수치 + `DESkillRowTable` 메타 + EvolutionDataTable) → `UDEAutoSkillBase::Activate` → `BuildContext`(`FDESkillContext`) → `ExecutePipeline`(`Behaviors[]`, `UDESkillBehavior` 서브클래스 `DEBehavior_*`) → 스폰 액터 `ADESkillActorBase` 계열(풀링) |
| 몬스터 | `ADEMonsterSpawnManager`(웨이브 DT, 자체 풀, Tick에서 업데이트 구동) → `UDEMonsterUpdateComponent`(이동과 분리를 중앙에서 일괄 처리, 몬스터 개별 Tick 없음), `ADEMonsterRepositionManager`, `FDEMonsterData`(DT) |
| 몬스터 사망 | `OnDying` → `ADEMonsterBase::StartDying` → `OnMonsterDeath` → `SpawnManager::OnMonsterDied`(드롭) → 다음 Tick에 `ExecuteFinalDeath` → 풀 반환 |
| 풀링/픽업/맵 | `UDEPoolSubsystem`(픽업, 스킬 액터), `UDEPickupManager`(자석), `ADEInfiniteMapManager`(3x3 타일 랩핑) |

## 스킬 제작 방식 (확정)

- **동작(Behavior)은 C++로, 조립은 BP에서 한다.** 새 스킬은 `UDEAutoSkillBase`의 BP 서브클래스로 만들고 `Behaviors` 배열을 에디터에서 조립한다. 스킬마다 C++ 클래스를 만들지 않는다.
- C++는 새로운 동작이 필요할 때 `DEBehavior_*`를 추가하는 데만 쓴다.
- **레거시:** `DESkill_*` C++ 서브클래스는 `InitBehaviors()`에서 Behavior를 하드코딩한다. 이들은 점진적으로 BP로 옮길 대상이다.
  - 주의: `UDEAutoSkillBase`의 `Behaviors.Empty()`가 주석 처리되어 있다. BP에 Behavior를 넣고 C++ `InitBehaviors()`도 추가하면 **두 번 실행된다.**
- `UDEActiveSkillBase`(BloodDrain)는 수동 스킬이고 이 프레임워크 밖에 있다.
- **Behavior 수치 프로퍼티 규칙 (2026-10-20):** 기본값은 -1이고, -1이면 Context(DT 컬럼 또는 OptionValues 키) 값을 따른다. 0 이상이면 그 Behavior만 그 값으로 덮어쓴다. `UDESkillBehavior::ResolveOverride`로 고른다. "맵 전체" 같은 모드는 -1에 싣지 않고 별도 bool로 둔다 (`SelectTargetsInRadius::bTargetAllMonsters`).
- **스폰 액터 소유:** 키당 하나인 액터는 `OwnedSkillActorMap` + `FindKeyedSkillActor`, 여러 개는 `RegisterSkillActor`. 둘 다 `IsActiveFor`로 소유를 확인한다. 약포인터가 유효해도 풀에 반환됐을 수 있다.

## 멀티플레이 대비

지금 리플리케이션을 추가하지는 않는다. 다만 새 코드에서 다음은 지킨다.

- `GetPlayerPawn(0)`, `GetPlayerController(0)`, `GetFirstPlayerController()`를 **새로 추가하지 않는다.** Owner, Instigator, 소유 Controller를 거쳐 접근한다.
- 게임 상태를 바꾸는 로직(데미지, 사망, 드롭, 경험치, 골드, 저장)은 나중에 권한 주체가 될 GameMode나 컴포넌트에 둔다. **UI나 위젯에 두지 않는다.** 예: 골드 정산과 저장은 `UDEResultWidget`이 아니라 `ADEGameMode_Stage::GameOver`가 한다.
- **레벨업 방식은 공유 레벨 + 모아서 동시 선택으로 정했다 (2026-10-20).** 경험치와 레벨은 파티가 공유한다. 레벨업하면 전원이 멈추고 모두 동시에 고르며, 전원이 고르면 재개한다. 느린 사람에 대비해 선택 타이머를 둔다. 친구끼리 하는 게임이라 잠깐 기다리는 건 괜찮다고 판단했다 (VS 온라인 co-op, Bounty of One과 같은 방향).
  - 지금은 레벨업 큐와 선택지를 `UDELevelUpManagerComponent`가, 월드를 멈출지는 `ADEPlayerController::ApplyPauseState`(멈춘 이유 집합 `EDEPauseReason`)가 정한다. 멀티로 갈 때 경험치, 레벨, 멈춤 판단은 GameMode(또는 GameState)로 올리고, 각 플레이어의 LevelUpManager는 자기 선택지 생성과 적용(`ApplyChoice` → Server RPC)만 맡는다.
  - 상자를 멀티에서 어떻게 할지는 아직 정하지 않았다.

## 컨벤션

- **인코딩:** `.h`/`.cpp`는 **UTF-8 with BOM**으로 저장한다. CP949는 쓰지 않는다. 2026-10-19에 전체 변환했고, `.editorconfig`로 강제한다.
  - `DEStatComponent.h`의 주석은 그 전부터 이미 손상되어 있었다.
- **include:** 사용하는 타입의 헤더는 직접 include한다. unity 빌드에서 이웃 파일 덕분에 통과하는 코드는 수정된 파일이 non-unity로 빌드될 때 깨진다.
- **데미지:** `UDEGameplayLibrary::ApplyCombatDamage`만 쓴다. `UGameplayStatics::ApplyDamage`는 크리티컬, OnHit/OnKill, 킬 집계를 건너뛰므로 쓰지 않는다.
- **풀링 액터:** `Destroy` 대신 풀로 반환하고, 재사용될 때 상태가 리셋되는지 확인한다.
- **로그:** 새 코드에서는 `LogTemp` 대신 `LogDarkEdenSurvivors`를 쓰는 것을 권장한다. Tick 안에서 Warning 로그를 찍지 않는다.

## 의도된 상태 (버그 아님)

- **플레이어 피격 (2026-10-20 무적 해제):** 몬스터 접촉 공격은 `ApplyCombatDamage`로 들어온다. 플레이어 `UDEHealthComponent`가 피격 후 무적시간(스탯 `Invulnerability`, 기본 0.2초, 뱀서는 0.24초)과 Armor(고정 감소, 0이 되지 않음)를 적용한다. 테스트할 때 무적은 콘솔 `God`(엔진 치트, `bCanBeDamaged`를 끈다)으로 켠다. 코드를 주석 처리해서 무적을 만들지 않는다.
  - 사망 흐름: HP 0 → `OnDying` → `ADECharacterBase::Die` → `OnPlayerDied` → `ADEGameMode_Stage::GameOver` → `EndStage`
- **사망 이벤트 두 단계:** `UDEHealthComponent`는 HP 0일 때 `OnDying`만 broadcast한다. 소유 액터는 `OnDying`에 사망 처리를 바인딩하고, 처리가 끝나면 `OnDeath`를 직접 broadcast한다 (몬스터 `ExecuteFinalDeath`, 플레이어 `Die`).

## 빌드

PowerShell에서 실행한다. Git Bash로 `Build.bat`을 호출하면 `C:\Program Files` 경로의 공백 때문에 실패한다.

```powershell
& "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" DarkEdenSurvivorsEditor Win64 Development "-Project=C:\Users\user\Documents\Unreal Projects\DarkEdenSurvivors\DarkEdenSurvivors.uproject" -WaitMutex -NoHotReloadFromIDE *> build.log
```

- `%LOCALAPPDATA%\UnrealBuildTool\Log.txt`는 다른 프로젝트(Kuaqe) 빌드와 공유되므로, 출력은 따로 리다이렉트해서 본다.
- 이 프로젝트의 에디터가 열려 있으면 Live Coding 때문에 빌드가 막힐 수 있다.
- **BP 참조는 C++에서 보이지 않는다.** "C++ 참조 0개"를 "안 쓰임"으로 단정하지 않는다. 삭제하기 전에 개발자에게 Reference Viewer로 확인해 달라고 요청한다.

## 할 일 목록

- `Todo.md`: 코드 작업 (정리 로드맵 포함). 작업을 끝내거나 새 문제를 발견하면 갱신한다.
- `MyTodo.md`: 개발자가 에디터에서 할 일 (BP, DT, Reference Viewer 확인, 인게임 테스트). 코드 작업 결과로 에디터 작업이 생기면 여기에 적는다.
- `Todo.md`의 "웨이브/스폰" 섹션은 개발자가 직접 정리할 예정이다. 요청받기 전에는 손대지 않는다.
- 주석에 ★, !!!, 이모티콘, `// =====` 배너를 넣지 않는다 (기존의 그런 주석은 Gemini 잔재).
