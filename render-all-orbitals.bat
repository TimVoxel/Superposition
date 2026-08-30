@echo off
setlocal enabledelayedexpansion

set MAX_N=4

for /L %%n in (1,1,%MAX_N%) do (
    set /a maxL=%%n-1
    for /L %%l in (0,1,!maxL!) do (
        set /a minM=-1*%%l
        for /L %%m in (!minM!,1,%%l) do (

            set "orbitalFile=resources/orbitals/hydrogen_n%%n_l%%l_m%%m"
            set "configName=hydrogen_n%%n_l%%l_m%%m"
            set "configPath=resources/hydrogen/!configName!.json"

            echo === Generating orbital n=%%n l=%%l m=%%m ===
            call run-ah %%n %%l %%m "resources/orbitals/hydrogen"
            if errorlevel 1 (
                echo Failed to generate orbital n=%%n l=%%l m=%%m
                exit /b 1
            )

            echo Writing config !configPath!
            (
                echo {
                echo     "name": "Scene - !configName!",
                echo     "objects": [
                echo         {
                echo             "type": "particleSystem",
                echo             "transform": {
                echo                 "pos": [0.0, 0.0],
                echo                 "scale": [1.0, 1.0]
                echo             },
                echo             "config": {
                echo                 "durationSeconds": 20.0,
                echo                 "fadeoutSeconds": 10.0,
                echo                 "maxSpawnRatePS": 250.0,
                echo                 "maxParticles": 1000,
                echo                 "lifetime": 5.0,
                echo                 "pointSize": 0.01,
                echo                 "sizeIncrease": 0.025,
                echo                 "amplitude": 0.0003,
                echo                 "frequency": 50.0,
                echo                 "color": [0.2, 0.2, 1.0, 1.0]
                echo             },
                echo             "waveFunction": {
                echo                 "type": "hydrogen",
                echo                 "orbital": "!orbitalFile!.bin",
                echo                 "timeStep": 0.01
                echo             }
                echo         }
                echo     ]
                echo }
            ) > "!configPath!"

            echo Rendering !configPath!
            call render "!configPath!"
            if errorlevel 1 (
                echo Failed to render !configPath!
                exit /b 1
            )
        )
    )
)

echo All done.
endlocal