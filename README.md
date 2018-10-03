Disjoint Shuffle
=====
David Lareau © 2018
No Affiliations

# Abstract

This paper proposes a novel approach at shuffling a looping sequence that minimizes caveats of naive solutions, keeps computation low, and offers a high degree of variance.

# Introduction

Imagine you are listening to the songs of an album, and set it up such that the songs be played in random order, and that the albums loops. What are your expectation once all songs have played? Should the songs be replayed in the same order, or should they be reshuffled? If you expect the later, are you ok with the last song you've heard being the next first song?

This paper compares various ways of generating a shuffled looping sequence, measuring statistics on the distance between duplicate entries.

# Algorithms

## Stateless

One of the simplest algorithm to think about when generating the next entry in a sequence is to select it randomly, without keeping track of any states. This opens up the possibility that an item be seen an infinite amount of times in a row, or never be seen at all.

The variance in the sequence is ideal, meaning the next entry is always a surprise.

## Shuffle

An improvement over the stateless method, is to keep the sequence shuffled in memory, visiting each entry once before reshuffling it and going over it again.

At the cost of memory, we now avoid the embarassing flaws of the stateless approach. All entries will be seen, and only once per full pass. The variance is much lower than the stateless approach because of these restrictions, but it's still ideal given the compromise.

Note that the common algorithm for shuffling (Fisher-Yate) is iterative, so you do not need to shuffle the whole list prior of reading the next entry. You can perform the shuffle one item at a time, meaning the size of the sequence does not affect the computation.

What becomes annoying are the looping boundaries, where an entry may be seen as soon as the next, or as far as a full pass.

## In Order

Let's compare our metrics with a sequence that loops, but isn't shuffled. Now the distance between each entry is ideal, but the variance is zero. In other words, listener knows which song will come next.

## Two Shuffles

In an effort to improve the minimum distance between the same entries in our shuffled looping sequence, we can split it in halves. The entries of the first half will be seen, followed by the entries of the second half. Shuffling is per halves, so the minimum distance between two entries is now half the size of the sequence.

This is fairly straightforward, and does not require much more computation per entry.

However, the variance is left to be desired. Over just one loop, the listener knows which songs are in which group and his well informed on what cannot possibly play next.

TODO image of two groups

## Two Shuffles (random size)

To improve the variance, we can split our sequence in ramdomly sized halves each loop.

TODO image of two groups over time, changing size.

## Disjoint Shuffle

To improve the variance even further, I propose we interlace the groups a little bit. The shuffle of an half is a bit more complex to implement now that its entries are disjoint in space, but the overall cost of computing the next entry in the sequence is still marginal.

TODO image of two disjoint group over time.

## Overlap Reshuffle

An alternate solution to the same problem is to shuffle two halves, then shuffle an overlaping region over both halves. This however, comes at the penalty of having to shuffle the list completely before use. This algorithm is not iterative.

# References

Fisher-Yate Shuffle
