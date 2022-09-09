Two Interlaced Shuffles in a Looping Sequence
=====
© 2018 David Lareau, Independent Scientist, Canada

# Abstract

This paper proposes a novel approach at shuffling a looping sequence that minimizes caveats of naive solutions, keeps computation low, and offers a high degree of variance.

# Introduction

Imagine you are listening to the songs of an album, and set it up such that the songs be played in random order [[9]](https://en.wikipedia.org/wiki/Shuffle_play), and that the album loops. What are your expectation once all songs have played? Should the songs be replayed in the same order, or should they be reshuffled? If you expect the later, are you okay with the last song you've heard being the next first song? Probably not, and neither do you want to hear it anytime soon.

This paper compares various ways of generating a shuffled looping sequence, measuring statistics on the distance between duplicate entries.

## Degenerate Cases

Even for a sequence as small as 2 items, it becomes important to use a smart algorithm to avoid having two items in a row. Most of the algorithm described in this paper split the sequence in groups. Care must be taken when choosing the size of the groups such that a small sequence doesn't break the algorithm's goals.

## Related Work

The algorithms described in this paper are presented to build-up to the less intuitive **Two Interlaced Shuffles**. The literature doesn't seem to cover shuffling looping sequence, so I designed and chose names for each algorithm.

In the wild, it is common for music players to have a *Random* or *Shuffle* feature. In VLC Media Player 3.0.4 on the desktop, the behavior of the *Random* toggle is like the **Shuffle** algorithm described below, where the same song can be heard twice in a row at the looping boundaries [[1]](https://github.com/videolan/vlc/blob/9bffed4c4d27b641bb035732348bf55dd2d3b835/src/playlist/thread.c#L172-L184).

Parole Media Player 1.0.1 (xfce's player) prevents playing the same song twice in a row, and also tries not to play any of the last three songs heard (when possible) [[5]](https://github.com/xfce-mirror/parole/blob/4d6b9e46f214606bcf1495e0dcb689acea527d90/src/parole-medialist.c#L1723-L1731). Aside from this small history, it is **stateless**. This behavior would be hard to infer without the source code. Rhythmbox [[6]](https://github.com/GNOME/rhythmbox)
 3.4.2, a Gnome player, seems to be using a **stateless** algorithm as well, as I could hear the same song multiple times before every song was heard in the sequence.

Both Windows Media Player [[7]](https://support.microsoft.com/en-ca/help/14209/get-windows-media-player) 12.0.16299.248 and iTunes [[8]](https://www.apple.com/itunes/) 12.3.2.35 go over all songs once before going over the sequence again, and both prevent the same song to be played twice in a row. However, at the sequence looping boundaries, it is possible to hear the same song again if at least one other song has been played in between (i.e. minimum distance is 2).

I am perplex that the spirit of the feature is obviously to play all the songs, shuffled in random order, and when it loops avoid having *too close* and *too far* duplicates, yet no player satisfies. The implementations all feel bizarre. I suspect the computer scientists involved got too literal with the name of the buttons, be it *random* or *shuffle*. In what scenario would you be happy to hear clusters of the same song? In the rare case where that is really what you seek, this freak need can be accommodated by allowing adding the same song multiple times to the playlist.

## It's not about Music

The application example of a music album (~12 songs) being played in a loop is not as contemporary as it used to be. This paper disregard that an easy way out of the problem is to add more songs, such that the shuffled sequence never needs to loop in one listening session.

The playlist example is easy to picture and straightforward to test in the wild, but it is not the actual problem I'm trying to solve. The problem is how to repeatedly shuffle a cyclic list and avoid *too close* and *too far* duplicates. Solutions involving spreading music genre uniformly [[4]](https://www.bbc.com/news/technology-31302312) have nothing to do with this problem. Using played count history is also not relevant.

A different example of a cyclic sequence could be spawning a random fruit in a video game for the player to pick up, then spawn another one when they do. It would be annoying to see three bananas in a row, or never see a single cherry.

# Algorithms

## Stateless

One of the simplest algorithm to think about when generating the next entry in a sequence is to select it randomly, without keeping track of any states. This opens up the possibility that an item be seen an infinite amount of times in a row, or never be seen at all. As mentioned in *Related Works*, this is how Parole Media Player, and Rhythmbox mostly behave.

The variance in the sequence is ideal, meaning the next entry is always a surprise.

```C
uint32_t next() {
  return arc4random_uniform(n);
}
```

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 1 | 0.01 | horrid |
| max | 946 | 9.46 | horrid |
| avg | 100.26 | 1.00 | ideal |
| std | 98.29 | 0.98 | ideal |

## Shuffle

An improvement over the stateless method, is to keep the sequence shuffled in memory, visiting each entry once before reshuffling it and going over it again. At the cost of memory, we now avoid the main flaw of the stateless approach. All entries will be seen once and only once per pass. The variance is much lower than the stateless approach because of these restrictions, but it's still ideal given the compromise.

Note that the common algorithm for shuffling (Fisher-Yate [[2]](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)) is iterative, so you do not need to shuffle the whole list prior of reading the next entry. You can perform the shuffle one item at a time, meaning the size of the sequence does not affect the computation.

The memory cost is not a real problem considering the entries have to be stored somewhere anyway, and that the algorithm can be done in-place. What becomes annoying are the looping boundaries, where an entry may be seen as soon as the next, or as far as a full pass. As mentioned in *Related Works*, this is how VLC, Windows Media Player and iTunes mostly work, sometime with a quick hack to prevent the same song twice in a row, but nothing more.

```C
uint32_t next() {
  if(i == n) i = 0;
  uint32_t j = i + arc4random_uniform(n - i);
  swap(i, j);
  return t[i++];
}
```

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 1 | 0.01 | horrid |
| max | 199 | 1.99 | horrid |
| avg | 100.01 | 1.00 | ideal |
| std | 40.88 | 0.41 | ideal |

## In Order

Let's compare our metrics with a sequence that loops, but isn't shuffled. Now the distance between each entry is ideal, but the variance is zero. In other words, the listener knows which song will come next.

```C
uint32_t next() {
  if(i == n) i = 0;
  return t[i++];
}
```

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 100 | 1.00 | ideal |
| max | 100 | 1.00 | ideal |
| avg | 100.00 | 1.00 | ideal |
| std | 0.00 | 0.00 | horrid |

## Two Disjoint Shuffles

In an effort to improve the minimum distance between the same entries in our shuffled looping sequence, we can split it in halves. The entries of the first half will be seen, followed by the entries of the second half. Shuffling is per halves, so the minimum distance between two entries is now half the total size of the sequence.

In other words, you now have two sequences that play one after another. There is no way the same song can be heard twice in a row anymore, since you need to at least visit all the entries of the other sequence before seeing it again.

Note that unless the sequence comes pre-shuffled, the first pass should shuffle over the whole sequence. We only begin using the two disjoint shuffles once we start looping, from the second pass and onwards. This is shown in the figure below by the full bar on top.

![](res/split.svg)

This solution is quite easy to implement, gives good enough results, and in all honesty if it was more prevalent I wouldn't be writing a paper about it. An implementation of this shuffle is available in [split.c](src/split.c).

The variance is left to be desired of course. Over just one loop, the listener knows which songs are in which group and is well-informed on what cannot possibly play next. The overall shuffle is now biased, so it becomes slightly harder to verify that the implementation of this algorithm is correct. You would need to verify that each half has been shuffled correctly in itself.

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times. The halves are 50 in length.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 51 | 0.51 | good |
| max | 149 | 1.49 | good |
| avg | 100.00 | 1.00 | ideal |
| std | 20.34 | 0.20 | good |

## Two Disjoint Shuffles (random size)

To improve the variance a little, we can split our sequence in randomly sized halves each loop. This way, entries can travel across over multiple pass. However, because one of the halves is now smaller, the variance is not improved that much per pass, and can only be appreciated after several loops.

![](res/split_r.svg)

An implementation of this shuffle is available in [split_r.c](src/split_r.c). The half point is now random, so in order to verify that the implementation of this algorithm is correct, one would need to control this random number.

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times. The halves have a random length between [25, 75].

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 29 | 0.29 | good |
| max | 168 | 1.68 | good |
| avg | 100.01 | 1.00 | ideal |
| std | 22.88 | 0.23 | good |

## Two Interlaced Shuffles

To improve the variance even further, I propose we keep the halves the same size, but interlace them. The size of the interlace from the center is random per pass, such that values can travel from one half to the other. I sometime call this algorithm *broken shuffle* because each disjoint shuffle is broken into two disconnected parts. If the interlace size is 0, then the pass is equivalent to the *Two Disjoint Shuffles* algorithm described earlier.

![](res/broken.svg)

The increase in variance is a bit of a hack, since we jump over a gap to consider positions further down the sequence. This makes the distance metric higher, but that's really because we are disregarding values in the gap. However, unlike the previous algorithm, here both halves are the same size, which means no group is left with a smaller internal variance.

An implementation of this shuffle is available in [broken.c](src/broken.c). In order to verify that the implementation of this algorithm is sound, one needs to know the random interlace size in each pass. The test file [verify.c](src/verify.c) does this and checks that the two groups are shuffled without bias.

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times. The halves are 50 in length, with the breaks being random between 1 and 25 from the center.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 28 | 0.28 | good |
| max | 174 | 1.74 | good |
| avg | 100.01 | 1.00 | ideal |
| std | 27.97 | 0.28 | good |

## Overlap Reshuffle

An alternate solution to the same problem is to shuffle two halves, then shuffle an overlapping region over both halves. This however, comes at the penalty of having to shuffle the list completely before use. This algorithm is not iterative.

Though shuffling the whole sequence before reading samples is quite silly in any context, it is sadly how code libraries are designed [[3]](https://docs.oracle.com/javase/9/docs/api/java/util/Collections.html#shuffle-java.util.List-). A playing card dealer does not need to shuffle the whole deck if it can simply pick five cards out randomly (as computers can do). Shuffling ahead of time is a human flaw.

![](res/overlap.svg)

```C
uint32_t next() {
  if(i == n) i = 0;
  if(i == 0) {
    shuffle(0, n / 2);
    shuffle(n / 2, n);
    shuffle(n / 4, 3 * n / 4);
  }
  return t[i++];
}
```

Distance statistics in a simulation with a sequence of 100 elements, looping 10,000 times. The halves are 50 in length, and the overlap covers 25 in each.

| Distance | Value | Normalized | Comment |
|:---:|:---:|:---:|:---:|
| min | 26 | 0.26 | good |
| max | 174 | 1.74 | good |
| avg | 100.00 | 1.00 | ideal |
| std | 27.09 | 0.27 | good |

# Conclusion

The algorithms described in this paper are nothing to brag about, but the media players I've tried in my life (on PC, cars, or ghetto blaster) always came short when it came to the loop/shuffle combined features. Simply preventing that a song be heard twice in a row, but not preventing much further than that feels cheap.

Though I'm proposing the **Two Interlaced Shuffles** algorithm, I'd be happy if at least the **Two Disjoint Shuffles** algorithm would be used more pervasively. Nevertheless, the interlaced algorithm gives a higher variance, remains an iterative algorithm, and comes at marginal computation cost.

The **Overlap Reshuffle** algorithm is easier to implement than the interlaced one, and applications that pre-shuffle the sequence to show it to the user, like iTunes does, could benefit from using it.

# References

[1] [VLC Media Player Random Algorithm](https://github.com/videolan/vlc/blob/9bffed4c4d27b641bb035732348bf55dd2d3b835/src/playlist/thread.c#L172-L184)

[2] [Fisher-Yate Shuffle](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)

[3] [java.util.Collections.shuffle()](https://docs.oracle.com/javase/9/docs/api/java/util/Collections.html#shuffle-java.util.List-)

[4] [How random is random on your music player? Dave Lee, BBC News. 2015-02-19](https://www.bbc.com/news/technology-31302312)

[5] [Parole Media Player Shuffle Algorithm](https://github.com/xfce-mirror/parole/blob/4d6b9e46f214606bcf1495e0dcb689acea527d90/src/parole-medialist.c#L1723-L1731)

[6] [RhythmBox](https://github.com/GNOME/rhythmbox)

[7] [Windows Media Player](https://support.microsoft.com/en-ca/help/14209/get-windows-media-player)

[8] [iTunes](https://www.apple.com/itunes/)

[9] [Wikipedia: Shuffle play](https://en.wikipedia.org/wiki/Shuffle_play)
