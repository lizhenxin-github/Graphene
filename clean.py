f = open("nohup.out","r")
out = open("result.txt","w")
line = f.readline()

substring = ["chunk_utilization11:","Total time:"]
workload = ["================"]
# allocator = ["****MAKALU*****","****RALLOC******","*****PMDK********","*****JEMALLOC*********","****TCMALLOC*****","*******NVMMALLOC******","*******BPMALLOC********"]
i=0
while line:
    for index in range(len(workload)):
        if (workload[index] in line):
            out.write(line)
            i=0
            continue 
    # for index in range(len(allocator)):
    #     if allocator[index] in line:
    #         out.write(line)
    for index in range(len(substring)):
        if substring[index] in line:
            result = line.split()
            # print(result)
            if index == 0:
                out.write(result[-3]+" "+result[-2]+" "+result[-1]+" "+"\n")
            elif index == 1:
                out.write(result[-2]+"\n")
    line = f.readline()

out.close()
f.close()
