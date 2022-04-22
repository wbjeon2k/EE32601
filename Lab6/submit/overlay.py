class matmul_overlay(Overlay):
    def __init__(self, bitfile_name, SIZE=64):
        super().__init__(bitfile_name)
        #self.saxpy_hw = self.krnl_saxpy2_0
        self.abuf = allocate((SIZE*SIZE*2, ) , dtype=np.float32)
        self.bbuf = allocate((SIZE*SIZE*2, ) , dtype=np.float32)
        self.obuf = allocate((SIZE*SIZE, ) , dtype=np.float32)
        #interrupt not implemented
    
    def python_float_to_uint(self, num):
        return ctypes.c_uint.from_buffer(ctypes.c_float(num)).value
    
    def register_map(self):
        return self.register_map
    
    def ip_dict(self):
        return self.saxpy_hw.ip_dict()
    
    def run(self, x, y):
        np.copyto(self.abuf, x.flatten())
        np.copyto(self.bbuf, y.flatten())
        #self.saxpy_hw.write(saxpy_hw.register_map.size.address, SIZE)
        #self.saxpy_hw.write(saxpy_hw.register_map.a.address, self.python_float_to_uint(a))
        
        self.dma_1.sendchannel.transfer(self.abuf)
        self.dma_2.sendchannel.transfer(self.bbuf)
        self.out_dma.recvchannel.transfer(self.obuf)
        self.dma_1.sendchannel.wait()
        self.dma_2.sendchannel.wait()
        self.out_dma.recvchannel.wait()
            
        return self.obuf
    
    def assert_by_sw(self, x, y):
        xy = np.matmul(x,y)
        xy_hw = self.run(x,y)
        comparison = np.isclose(xy.flatten(),xy_hw)
        for i in comparison:
            assert(i == True)
            
        return True
    
MATMUL = matmul_overlay("matmul3.bit")
MATMUL.run(a,b)
MATMUL.assert_by_sw(a,b)