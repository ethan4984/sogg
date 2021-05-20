# sogg

This is a very simple digit recognition program written from scratch. This uses the MNIST data set, and it uses the k-nearest-neighbor algorithm for
digit identification. Optimally we can achieve around 90% successful recognition.

# Running

To run this, download and unzip the MNIST data set and put them in `./data/`, or manually edit the MNIST paths in the Makefile. Then do `make run` 

Here is some example output 
```
Predicting 7, answer: 7
Predicting 2, answer: 2
Predicting 1, answer: 1
Predicting 0, answer: 0
Predicting 4, answer: 4
Predicting 1, answer: 1
Predicting 4, answer: 4
Predicting 9, answer: 9
Predicting 9, answer: 5
Predicting 9, answer: 9
Predicting 0, answer: 0
Predicting 6, answer: 6
Predicting 9, answer: 9
...
```
