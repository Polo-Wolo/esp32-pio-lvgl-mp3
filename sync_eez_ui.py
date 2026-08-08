#!/usr/bin/env python3
"""
Copie le dossier "ui" généré par EEZ Studio (EEZ-BUILD-PATH/src/ui)
vers le dossier lib du projet PlatformIO (PLATFORMIO-PROJECT-PATH/lib/ui),
en remplaçant l'ancien dossier s'il existe.

Usage:
    python sync_eez_ui.py <EEZ_BUILD_PATH> <PLATFORMIO_PROJECT_PATH>

Exemple:
    python sync_eez_ui.py ./mon_projet_eez ./mon_projet_platformio
"""

import argparse
import shutil
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description="Copie le dossier ui d'un build EEZ Studio vers un projet PlatformIO."
    )
    parser.add_argument("eez_build_path", help="Chemin du build du projet EEZ Studio")
    parser.add_argument("platformio_project_path", help="Chemin du projet PlatformIO")
    args = parser.parse_args()

    eez_build_path = Path(args.eez_build_path).resolve()
    platformio_project_path = Path(args.platformio_project_path).resolve()

    source_ui = eez_build_path / "src" / "ui"
    dest_lib = platformio_project_path / "lib"
    dest_ui = dest_lib / "ui"

    # Vérifications
    if not eez_build_path.is_dir():
        sys.exit(f"Erreur : le chemin du build EEZ Studio n'existe pas : {eez_build_path}")

    if not source_ui.is_dir():
        sys.exit(f"Erreur : dossier 'ui' introuvable dans le build EEZ Studio : {source_ui}")

    if not platformio_project_path.is_dir():
        sys.exit(f"Erreur : le chemin du projet PlatformIO n'existe pas : {platformio_project_path}")

    # Crée le dossier lib s'il n'existe pas encore
    dest_lib.mkdir(parents=True, exist_ok=True)

    # Supprime l'ancien dossier ui s'il existe
    if dest_ui.exists():
        print(f"Suppression de l'ancien dossier : {dest_ui}")
        shutil.rmtree(dest_ui)

    # Copie le nouveau dossier ui
    print(f"Copie de {source_ui} -> {dest_ui}")
    shutil.copytree(source_ui, dest_ui)

    print("Terminé : le dossier 'ui' a été synchronisé avec succès.")


if __name__ == "__main__":
    main()