# Run Test framework for EFR32 #

Clean old tests result

    $ cd circuitpython/ports
    $ make BOARD=brd2601b tests-clean

Rebuild to generate test script to **/circuitpython/tests/** folder

    $ make BOARD=brd2601b V=2

Go to **/circuitpython/tests/** folder

    $ cd ../../tests/

Run test script

    $ ./silabs-run-tests.py --target efr32 --device /dev/ttyACM0

After completely run test script, check the test result in the excel file  **circuitpython/tests/result.xlsx**
