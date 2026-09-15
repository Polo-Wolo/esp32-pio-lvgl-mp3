# pio-lvgl
PlatformIo ESP32 display with LVGL


##### Sources
###### Micro-controller link
https://docs.waveshare.com/ESP32-S3-AMOLED-1.91?variant=ESP32-S3-Touch-AMOLED-1.91-M

##### Installation inspiration
https://www.youtube.com/watch?v=ofa7ujpaJgU


### Windows unpacking issue
I had an issue unpacking `esp32-core`, this is due to windows path character limited.

Extend windows paths :

```sh
reg add "HKLM\SYSTEM\CurrentControlSet\Control\FileSystem" /v LongPathsEnabled /t REG_DWORD /d 1 /f
```

Delete `C:/Users/YOU/.platformio/.cache`

Restart you PC and try again.
## Commandes audio asynchrones

La tache Arduino (`setup` / `loop`) possede le decodeur et le Jukebox.
LVGL envoie des commandes a une file FreeRTOS FIFO de 24 elements : lecture,
suivant, precedent, pause, position, volume, shuffle et repeat.

- `player.next()` et les autres commandes renvoient `true` si l'envoi est
  accepte, `false` si la file est pleine ou non initialisee. Ce resultat ne
  garantit pas la reussite de la lecture. Les handlers UI signalent les refus
  dans le moniteur serie ; ils n'attendent pas une place dans la file.
- Une operation utilisateur est executee par tour, entre deux appels au decodeur.
  Jusqu'a quatre Seek ou SetVolume consecutifs peuvent etre remplaces par leur
  derniere valeur. Les changements relatifs de volume restent tous executes.
- `previousOrRestart()` decide sur le temps reel : au-dela de trois secondes,
  retour au debut ; sinon piste precedente, comme auparavant.
- `player.state()` fournit une copie coherente du dernier etat publie (toutes
  les 50 ms, hors duree des operations audio). Les getters lisent cette copie.
  Ne pas lire un getter juste apres un envoi pour confirmer son execution.
- Le seul mutex applicatif audio protege cette copie ; aucun appel decodeur,
  SD ou LVGL n'est execute sous ce verrou. Le mutex propre a LVGL reste requis.
- Charger/attacher le Jukebox dans setup. Apres cela, passer par AudioPlayer
  depuis l'UI, y compris pour shuffle/repeat et les metadonnees.
- EOF est traite hors callback, avant la prochaine commande utilisateur.
  A un changement de piste, les anciens evenements sont purges apres arret.
  Cette integration repose sur la version epinglee d'ESP32-audioI2S, dont
  `get_info()` livre les callbacks dans `audio.loop()`.

### Verification sur la carte

Apres `pio run`, televerser puis verifier avec le moniteur serie :

1. Demarrage avec SD : lecture et tags de la premiere piste.
2. Pause/reprise et appuis rapides sur suivant/precedent : ordre conserve,
   interface utilisable et tags correspondant au morceau courant.
3. Precedent avant/apres trois secondes : piste precedente / retour au debut.
4. Gestes de volume repetes : tous les pas comptes, limites 0 et 21.
5. Glissements de position rapides : curseur et temps suivent le doigt, un
   seul seek est envoye au relachement. Le curseur reste sur la cible jusqu'au
   retour du decodeur (maximum deux secondes), sans animation de retour ni
   changement de piste par propagation du geste. Tester aussi un simple tap,
   deux glissements successifs et un seek refuse en pause.
6. Fin naturelle de piste, repeat OFF/ALL/ONE et shuffle : avancement attendu,
   sans ancien EOF provoquant un saut supplementaire apres changement manuel.
7. Rafale depassant la capacite : refus signale, puis commandes a nouveau
   acceptees lorsque la file se vide ; aucun blocage de LVGL.

La compilation ne valide pas les temps de reponse, l'absence de coupures audio
ni le comportement du materiel : ces points necessitent cet essai sur carte.
