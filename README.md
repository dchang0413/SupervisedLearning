# SupervisedLearning
Supervised learning using ID3 algorithm for CSCI 4350 Introduction to Artificial Intelligence
Overview

Develop a software agent in C++ to learn an ID3 decision tree from labeled classification data.
Procedure

    Create a C++ program (id3.cpp) which creates an ID3 decision tree to classify a set of input training data and then reports the classification performance on a separate set of input testing data.
        The program should take 3 command-line arguments (integer: the number of real-valued features in the data set, string: input training data filename, string: input testing data filename)
        The program should read in the training data file (one training example per line, see below)
        The program should read in the testing data file (one testing example per line, see below)
        Each line will contain the specified number of real feature values and a single interger value (the class label) at the end
        The program should build an ID3 decision tree by sorting the training data by each feature/attribute and determining potential binary split points based on attribute value changes (average the two values to make the split) and maximum information gain
        Ties (in maximum information gain) should be broken by attribute order (left to right) and then attribute value (smallest to largest) as found in the input file
        Terminal nodes are created when
            the probability of one of the class labels is 1 (all others zero) or
            there are no more potential split points found among the attributes (use a majority class label vote, with ties in favor of the smaller integer label)
        Once the decision tree has been created, each of the testing examples should be classified using the resulting decision tree
        The program should then output the number of testing examples classified correctly by the decision tree
