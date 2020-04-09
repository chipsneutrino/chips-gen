# chips-gen

CHIPS cosmic event generation code. Using the cosmic ray event generator [CRY](https://nuclear.llnl.gov/simulation/main.html)

## Building

You need an install of CRY. This is provided by the chips-env repository.
The first time you run gen-setup.sh it will build chips-gen, any further changes will require a 'make' command

```
$ source gen-setup.sh
```

## Running cosmicGen

```
$ source gen-setup.sh
$ cosmicGen [configfile] [numEvents] [geometry] [seed] [forTraining]
```

## Cleaning Everything Up

To remove all artifacts and return to the base state run...

```
$ source gen-tidy.sh
```
