# Robot Admin AI OS — Owner Manual V70.0

## ಸ್ಥಿತಿ
ಈ document final stable releaseಗೆ ಮೊದಲು owner verification guide. ಇದು automatic production enable instruction ಅಲ್ಲ.

## Install / Start / Stop
- Approved stable source ಮಾತ್ರ ಬಳಸಿ.
- Start ಮಾಡುವ ಮೊದಲು backup status, project isolation, Trading firewall ಮತ್ತು production gate ಪರಿಶೀಲಿಸಿ.
- Stop/Pause ಸಂದರ್ಭದಲ್ಲಿ queue state persist ಆಗಿದೆಯೇ verify ಮಾಡಿ.

## Dashboard
Dashboardನಲ್ಲಿ Current Level, Target Level, Current Worker, Test Evidence, Backup Status, Audit History, Resource Health, Emergency Stop ಮತ್ತು Trading Firewall status ಕಾಣಬೇಕು.

## Project Creation
ಹೊಸ projectಗೆ unique Project ID, permissions, memory, logs, backup path ಮತ್ತು policy version ಕಡ್ಡಾಯ. Cross-project access default BLOCK.

## Safe Auto-Level
Safe Auto-Level owner approval ಇಲ್ಲದೆ execute ಆಗಬಾರದು. Required tests, acceptance proof, verified backup ಮತ್ತು rollback readiness PASS ಆದ ನಂತರ ಮಾತ್ರ next levelಗೆ ಹೋಗಬೇಕು.

## Approval / Rejection
Critical actionಗೆ owner authentication, scoped approval, expiry, one-time token ಮತ್ತು Kannada reason ಕಡ್ಡಾಯ. Replay/reused approval BLOCK.

## Backup / Restore
ಪ್ರತಿ levelಗೂ checkpoint ಬೇಕು. Restore automatic ಅಲ್ಲ. Restore proposalಗೆ backup verification, rollback target ಮತ್ತು owner approval ಬೇಕು.

## Emergency Recovery
Battery/heat/storage risk, crash, incomplete write ಅಥವಾ corrupted state ಕಂಡರೆ system PAUSE ಆಗಬೇಕು. Last-known-good state restore owner-controlled ಆಗಿರಬೇಕು.

## Troubleshooting
Build/Test FAIL ಆದರೆ COMPLETED status ಕೊಡಬಾರದು. FAILED → DIAGNOSED → REPAIRED → RETESTED → ROLLBACK/PAUSE flow ಅನುಸರಿಸಬೇಕು.

## Release
Development → Testing → Canary → Stable → Archived/Rollback. Stableಗೆ release notes, checksums, compatibility report, known issues, recovery instructions ಮತ್ತು owner approval ಕಡ್ಡಾಯ.

## Permanent Safety Locks
Trading write access BLOCK. Cross-project write BLOCK. Runtime install OFF. Auto merge OFF. Auto tag OFF. Auto publish OFF. APK build OFF until explicit owner approval. Production gate CLOSED until approved runtime integration.
