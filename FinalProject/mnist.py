## run this on pnyq..

import numpy as np
import time
import torch

#from scipy.interpolate import interp1d

mnist = np.load("mnist-original.npy", allow_pickle= True)

X = mnist.item().get("data").T / 255
y = mnist.item().get("label")[0]
'''
weights = np.load('model.npy',allow_pickle=True)

fc1w = weights.item().get('fc1w')
fc1b = weights.item().get('fc1b')

fc2w = weights.item().get('fc2w')
fc2b = weights.item().get('fc2b')

fc3w = weights.item().get('fc3w')
fc3b = weights.item().get('fc3b')
'''
import torch
model = torch.load("MNIST.pth")

'''
numpydict = {}
numpydict['fc1w'] = np.array(model['fc1.weight'])
numpydict['fc1b'] = np.array(model['fc1.bias'])
numpydict['fc2w'] = np.array(model['fc2.weight'])
numpydict['fc2b'] = np.array(model['fc2.bias'])
numpydict['fc3w'] = np.array(model['fc3.weight'])
numpydict['fc3b'] = np.array(model['fc3.bias'])

np.save('model.npy',numpydict,allow_pickle = True)
'''


fc1w = np.array(model['fc1.weight'])
fc1b = np.array(model['fc1.bias'])

fc2w = np.array(model['fc2.weight'])
fc2b = np.array(model['fc2.bias'])

fc3w = np.array(model['fc3.weight'])
fc3b = np.array(model['fc3.bias'])
batch_size = 100

# assume hw can do only int8 multiplication

#print(fc1w)
#print(fc1w.shape)


# assume hw can do only int8 multiplication

def sign_bit(a):
    if(a > 0):
        return 1
    else:
        return -1
    
def sign_bit2(a):
    if(a > 0):
        return 1
    else:
        return 0
    
def image_quantize(a):
    if(a<0.3):
        return 0
    else:
        return 1


sign_bit_map = np.vectorize(sign_bit)
sign_bit_map2 = np.vectorize(sign_bit2)
image_quantize_map = np.vectorize(image_quantize)


def relu(a):
    return np.maximum(0, a)

def parameter_converter(fc_weight):
    fc_q = sign_bit_map2(fc_weight)
    fc_q_t = fc_q
    print("fc_q_t shape: ",fc_q_t.shape)
    print(fc_q_t)
    fc_shape = fc_q_t.shape
    row = fc_shape[0]
    col = fc_shape[1]
    ret = np.ndarray(((fc_q_t.shape[0] // 32)+1, fc_q_t.shape[1]), dtype=np.int32)
    print("ret shape: ", ret.shape)
    for i in range(col):
        tmp = 0
        for j in range(row):
            idx = j % 32
            tmp += (fc_q_t[j][i] << idx)
            if(j == row - 1 or idx == 31):
                ret[j//32][i] = tmp
                tmp = 0
    ret_np = ret.flatten()
    return ret_np

def tc_gen(row,col):
    ret = np.zeros((row,col))
    for i in range(row):
        for j in range(col):
            ret[i][j] = (i+j)%2
    return ret
            

def convert_and_save():

    numpydict = {}
    
    fc1w_q = sign_bit_map2(fc1w)
    fc1w_q = fc1w_q.T
    fc1b_q = sign_bit_map2(fc1b)
    numpydict['fc1w_q'] = fc1w_q
    numpydict['fc1b_q'] = fc1b_q
    
    fc2w_q = sign_bit_map2(fc2w)
    fc2w_q = fc2w_q.T
    fc2b_q = sign_bit_map2(fc2b)
    numpydict['fc2w_q'] = fc2w_q
    numpydict['fc2b_q'] = fc2b_q
    
    fc3w_q = sign_bit_map2(fc3w)
    fc3w_q = fc3w_q.T
    fc3b_q = sign_bit_map2(fc3b)
    numpydict['fc3w_q'] = fc3w_q
    numpydict['fc3b_q'] = fc3b_q
    
    np.save('model_q.npy',numpydict,allow_pickle = True)


def feed_foward(X0):
    fc1w_q = sign_bit_map(fc1w)
    fc1b_q = sign_bit_map(fc1b)
    
    #print(X0.mean())
    X0 = image_quantize_map(X0)
    #print(X0)
    
    #X1 = np.matmul(X0, fc1w.T) + np.tile(fc1b, (batch_size, 1))
    #X1 = np.matmul(X0, fc1w_q.T) + np.tile(fc1b_q, (batch_size, 1))
    X1_prime = np.matmul(X0, fc1w_q.T)
    
    #print("X1 avg: ", np.mean(X1_prime), "X1 std: ", np.std(X1_prime))
    
    #print("X1 shape: ", X1.shape, "X1_prime shape: ",X1_prime.shape)

    #A1 = np.tanh(X1)
    #A1 = relu(X1)
    #A1 = X1
    A1 = X1_prime 

    A1_q = sign_bit_map(A1)
    fc2w_q = sign_bit_map(fc2w)
    fc2b_q = sign_bit_map(fc2b)

    HW_result = np.matmul(A1_q, fc2w_q.T)

    #X2 = HW_result + np.tile(fc2b, (batch_size, 1))
    #X2 = HW_result + np.tile(fc2b_q, (batch_size, 1))
    X2_prime = HW_result
    

    #A2 = relu(X2)
    A2 = X2_prime
    #A2 = np.tanh(X2)
    A2_q = sign_bit_map(A2)
    fc3w_q = sign_bit_map(fc3w)
    fc3b_q = sign_bit_map(fc3b)

    #X3 = np.matmul(A2, fc3w_q.T) + np.tile(fc3b, (batch_size, 1))
    #X3 = np.matmul(A2_q, fc3w_q.T) + np.tile(fc3b_q, (batch_size, 1))
    X3_prime = np.matmul(A2_q, fc3w_q.T)
    
    #print(X3_prime)
    return X3_prime


def feed_foward2(X0):
    X1 = np.matmul(X0, fc1w.T) + np.tile(fc1b, (batch_size, 1))
    A1 = np.tanh(X1)
    
    size_of = 16
    A1 = np.multiply(A1,size_of)
    A1 = np.add(A1,3)
    A1_q = np.clip((A1*size_of).astype(np.int),-8,7)
    fc2w_q = np.clip((fc2w.T*size_of).astype(np.int),-8,7)
    HW_result = np.matmul(A1, fc2w.T) / (size_of*size_of)
    HW_result = np.clip(HW_result,-8,7) 
    
    # hw int8 : -128 ~ 127
    
    #X2 = np.matmul(A1, fc2w.T) + np.tile(fc2b, (batch_size, 1))
    X2 = HW_result + np.tile(fc2b, (batch_size, 1))
    A2 = np.tanh(X2)

    X3 = np.matmul(A2, fc3w.T) + np.tile(fc3b, (batch_size, 1))
    return X3


#convert_and_save()
def quantize_single_image(X0):
    X0 = image_quantize_map(X0)
    assert (X0.shape == (784,))
    return X0

def quantize_mnist(X,batch_size):
    ret = []
    for i in range(len(X)//batch_size):
        batch = []
        for j in range(10):
            batch.extend(quantize_single_image(X[i*batch_size + j]))
        ret.append(batch)
        print(f'Currently {i}/7840')
    
    ret = np.array(ret)
    print(ret.shape)
    assert (ret.shape == (len(X)//batch_size,7840))
    return ret
        
    
'''
prediction = []
for idx in range(len(X)//batch_size):
    xs = X[batch_size * idx:batch_size * idx + batch_size]
    ys = y[batch_size * idx:batch_size * idx + batch_size]
    outputs = feed_foward(xs)
    for output, yk in zip(outputs, ys):
        prediction.append(np.argmax(output) == (yk))

score = np.mean(prediction) * 100

print(score)
'''

quantize_single_image(X[0])

numpydict = {}

mnist_q = quantize_mnist(X,10).astype(np.uint32)
numpydict['image'] = mnist_q
numpydict['label'] = y

np.save('image_q.npy',numpydict,allow_pickle = True)

#print(np.mean(fc2w))
#print(np.std(fc2w))
'''
tc = tc_gen(33,5)
conv = parameter_converter(tc)
print(conv)
'''

'''
conv_fc1w = parameter_converter(fc1w)
print(conv_fc1w.shape)
print(conv_fc1w)
print(conv_fc1w[3199])
'''
