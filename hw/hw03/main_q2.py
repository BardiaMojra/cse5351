import numpy as np
import matplotlib
from matplotlib import pyplot as plt

from nbug import *
from pdb import set_trace as st



''' matplotlib config '''
matplotlib.pyplot.ion()
plt.style.use('ggplot')
np.set_printoptions(precision=3)


primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97,
  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
	211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
	307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
 	401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499,
503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599,
	601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691,
 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797,
809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887,
907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997 ]


def calc_ser_time(n,x=1):
  # no communication time
  ps = [p for p in range(p_min,p_max+1)]
  k = len(primes)
  total_strike_time = 0
  for prime in primes:
    total_strike_time += abs(((n+1)-(prime**2))/prime)
  total_time = total_strike_time + 78498
  return total_time

def calc_par_times(p_min,p_max,n,x,lamb):
  ps = [p for p in range(p_min,p_max+1)]
  k = len(primes)
  lamb = lamb * x # unit communication time
  total_times = list()
  for p in ps:
    total_strike_time = 0.0
    for prime in primes:
      total_strike_time += (n/p)/prime
    # total_strike_time = int(total_strike_time * x)
    total_strike_time = (total_strike_time * x)
    total_comm_time = k*(p-1) * lamb
    total_time = total_comm_time + total_strike_time
    print(f'processors: {p}')
    print(f'total_strike_time: {total_strike_time}')
    print(f'total_comm_time: {total_comm_time}')
    log = np.asarray([p, total_time])
    total_times.append(log)
  total_times = np.asarray(total_times).reshape(-1,2)
  # print(f'total_times: {total_times}')
  return total_times

def calc_spdup_curve(times:np.ndarray, ser_time):
  times[:,1] = ser_time/times[:,1]

  return times


p_min = 2 # min processors
p_max = 32 # max processors
x = 1 # unit striking time
n = 1000000 # prime number smaller than n


# get serial time
ser_time = calc_ser_time(n,x)
nprint('serial time', ser_time)

# lambda = 500X
lamb = 500
times_500x = calc_par_times(p_min,p_max,n,x,lamb)
spdup_500x = calc_spdup_curve(times_500x,ser_time)

# lambda = 100X
lamb = 100
times_100x = calc_par_times(p_min,p_max,n,x,lamb)
spdup_100x = calc_spdup_curve(times_100x,ser_time)

# lambda = 100X
lamb = 1
times_1x = calc_par_times(p_min,p_max,n,x,lamb)
spdup_1x = calc_spdup_curve(times_100x,ser_time)


nprint('500X speedups:', spdup_500x)
nprint('100X speedups:', spdup_100x)
nprint('1X speedups:', spdup_1x)

plt.figure()
plt.title("Lambda = 500X")
plt.xlabel("number of processors")
plt.ylabel("speedup")
plt.plot(spdup_500x[:,0], spdup_500x[:,1], label='500X')
plt.legend()
plt.show()

plt.figure()
plt.title("Lambda = 100X")
plt.xlabel("number of processors")
plt.ylabel("speedup")
plt.plot(spdup_100x[:,0], spdup_100x[:,1], label='100X')
plt.legend()
plt.show()


plt.figure()
plt.title("Lambda = 1X")
plt.xlabel("number of processors")
plt.ylabel("speedup")
plt.plot(spdup_1x[:,0], spdup_1x[:,1], label='1X')
plt.legend()
plt.show()

res = np.concatenate((spdup_1x,spdup_100x,spdup_500x), axis=1)
res = np.delete(res, [2,4], axis=1)
npprint('res',res)
np.savetxt('res.csv', res, delimiter=',')
st()

# EOF
