
from jetson_utils import cudaAllocMapped


class depthBuffers:
    def __init__(self, args):
        self.args = args
        self.depth = None
        self.composite = None
        
        self.use_input = "input" in args.visualize
        self.use_depth = "depth" in args.visualize
            
    def Alloc(self, shape, format):
        depth_size = (shape[0] * self.args.depth_size, shape[1] * self.args.depth_size)
        composite_size = [0,0]
        
        if self.depth is not None and self.depth.height == depth_size[0] and self.depth.width == depth_size[1]:
            return
            
        if self.use_depth:
            composite_size[0] = depth_size[0]
            composite_size[1] += depth_size[1]
            
        if self.use_input:
            composite_size[0] = shape[0]
            composite_size[1] += shape[1]

        self.depth = cudaAllocMapped(width=depth_size[1], height=depth_size[0], format=format)
        self.composite = cudaAllocMapped(width=composite_size[1], height=composite_size[0], format=format)
        