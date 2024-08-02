# BL 81.21.1110

Release Date: 11th November, 2021

Author

	~Changyong Yu (Northeastern University in CHINA)
	~Tianmei Ren (Northeastern University in CHINA)
	~Wenyu Li (Northeastern University in CHINA)
1.Introduction

BL is an index，it can be used for answering reachability query on very large graphs.

2.Test Data

    Test data obtained from related papers.

3.Building Notes

Our directory contains only a content : 1.codes to construct BL index for handling reachability query.

The directory named #src# consisted of the code necessary to construct the BL index.
To construct the BL, change the directory to src and type

	$ make

After that, you can type commands to run tests using BL. BL provides 9 methods for constructing,querying,filtering and comparing of large-scale graphs.

4.Usage Notes

1)I method. Generate BL index.

    $ ./BL -M <method> -g <input_graph_filename> -tl 1

    I method constructs topological level of the graph. It output one file, take input_graph_filename = citeseer.gr for example, it will output one file, named "citeseer.gr.index". It is the file saving topological level. The index in the files are sorted with the ascending order. 
	
    $ ./BL -M <method> -g <input_graph_filename> -rand 1
	
    I method constructs minBL index of the graph. It output one file, take input_graph_filename = citeseer.gr for example, it will output one file, named "citeseer.gr.index". It is the file saving minBL index. The index in the files are sorted with the ascending order. 

    $ ./BL -M <method> -g <input_graph_filename> -tfd 1
	
    I method constructs tfd-minBL index of the graph. It output one file, take input_graph_filename = citeseer.gr for example, it will output one file, named "citeseer.gr.index". It is the file saving tfd-minBL index. The index in the files are sorted with the ascending order. 

    $ ./BL -M <method> -g <input_graph_filename> -tfdd 1 -d <delta>
	
    I method constructs ctfd-minBL index of the graph. It output one file, take input_graph_filename = citeseer.gr and d = 10000 for example, it will output one file, named "citeseer.gr.index". It is the file saving ctfd-minBL index. The index in the files are sorted with the ascending order. 

    $ ./BL -M <method> -g <input_graph_filename> -randk 1 -k <size>
	
    I method constructs k-minBL index of the graph. It takes the first k nubmer from minBL index. It output one file, take input_graph_filename = citeseer.gr and k = 2 for example, it will output one file, named "citeseer.gr.index". It is the file saving k-minBL index. The index in the files are sorted with the ascending order. 

    $ ./BL -M <method> -g <input_graph_filename> -randk 2 -k <size>
	
    I method constructs k-minBL index of the graph. It takes the first k nubmer from tfd-minBL index. It output one file, take input_graph_filename = citeseer.gr and k = 2 for example, it will output one file, named "citeseer.gr.index". It is the file saving k-minBL index. The index in the files are sorted with the ascending order. 
    
    $ ./BL -M <method> -g <input_graph_filename> -randk 3 -k <size>

    I method constructs k-minBL index of the graph. It takes the first k nubmer from ctfd-minBL index. It output one file, take input_graph_filename = citeseer.gr and k = 2 for example, it will output one file, named "citeseer.gr.index". It is the file saving k-minBL index. The index in the files are sorted with the ascending order.

Running test:

    $ ./BL -M I -g citeseer.gr -tl 1

    The above commond takes "citeseer.gr" file as input. The file is calculating topological level of program BL. And it will output "citeseer.gr.index" file saving the topological level with each as an unsigned.
	
    $ ./BL -M I -g citeseer.gr -rand 1
    
    The above commond takes "citeseer.gr" file as input. The file is calculating minBL index of program BL. And it will output "citeseer.gr.index" file saving the index of minBL with each as an unsigned.

    $ ./BL -M I -g citeseer.gr -tfd 1
	
    The above commond takes "citeseer.gr" file as input. The file is calculating tfd-minBL index of program BL. And it will output "citeseer.gr.index" file saving the index of tfd-minBL with each as an unsigned.

    $ ./BL -M I -g citeseer.gr -tfdd 1 -d 10000
	
    The above commond takes "citeseer.gr" file as input. The file is calculating ctfd-minBL index of program BL. And it will output "citeseer.gr.index" file saving the index of ctfd-minBL with each as an unsigned.
	
    $ ./BL -M I -g citeseer.gr -randk 1 -k 2
    
    The above commond takes "citeseer.gr" file as input. The file is calculating k-minBL index of program BL. And it will output "citeseer.gr.index" file saving the first 2 number of minBL with each as an unsigned.
    
    $ ./BL -M I -g citeseer.gr -randk 2 -k 2
    
    The above commond takes "citeseer.gr" file as input. The file is calculating k-minBL index of program BL. And it will output "citeseer.gr.index" file saving the first 2 number of tfd-minBL with each as an unsigned.
    
    $ ./BL -M I -g citeseer.gr -randk 3 -k 2
    
    The above commond takes "citeseer.gr" file as input. The file is calculating k-minBL index of program BL. And it will output "citeseer.gr.index" file saving the first 2 number of ctfd-minBL with each as an unsigned.
    
2)G method. Calculate the random query pair of the graph.
 
    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName>
 
    G method calculates the random query pair. If source nodes can reach destination nodes, it output source nodes, destination nodes and 1. If source nodes can't reach destination nodes, it output source nodes, destination nodes and 0.
 
Running test:
 
    $ ./BL -M G -g citeseer.gr -geneFileName citeseer.query
 
    The above commond calculates random query pair of the graph. 
 	
3)N method. Calculate the negative query pair of the graph.
  
    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName> -index <indexFileName> -tl 1
  
    N method calculates the negative query pair based on topological level. It output source nodes, destination nodes and 0.
  	
    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName> -index <indexFileName> -rand 1
    
    N method calculates the negative query pair based on minBL index. It output source nodes, destination nodes and 0.

    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName> -index <indexFileName> -randk 1
    
    N method calculates the negative query pair based on k-minBL index. It output source nodes, destination nodes and 0.
    
    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName> -index <indexFileName> -tfd 1
	
    N method calculates the negative query pair based on tfd-minBL index. It output source nodes, destination nodes and 0.
    	
    $ ./BL -M <method> -g <input_graph_filename> -geneFileName <-geneFileName> -index <indexFileName> -rand 1
	
    N method calculates the negative query pair based on ctfd-minBL index. It output source nodes, destination nodes and 0.
  
Running test:
  
    $ ./BL -M N -g citeseer.gr -geneFileName citeseer.n -index citeseer.gr.index -tl/-rand/-randk/tfd/tfdd 1
  
    The above commond calculates negative query pair of the graph based topological level/minBL index/k-minBL index/tfd-minBL index/ctfd-minBL index.  

4)P method. Calculate the positive query pair of the graph.
  
    $ ./BL -M <method> -geneFileName <-geneFileName>
  
    P method calculates the positive query pair based on BFS. It output source nodes, destination nodes and 1. 
  	  
Running test:
  
    $ ./BL -M P -geneFileName citeseer.p
  
    The above commond calculates positive query pair of the graph. 
 
5)Q method. Handle query.

    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -tl 1

    Q method addresses query based on topological level.
	
    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -rand 1
    
    Q method addresses query based on minBL index. 
    
    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -randk 1
    
    Q method addresses query based on k-minBL index.
    
    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -tfd 1
    
    Q method addresses query based on tfd-minBL index.
    
    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -tfdd 1
    
    Q method addresses query based on ctfd-minBL index.

Running test:

    $ ./BL -M Q -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -tl 1

    The above commond addresses query based on topological level.
	
    $ ./BL -M Q -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -rand 1
    
    The above commond addresses query based on minBL index.
    
    $ ./BL -M Q -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -randk 1
        
    The above commond addresses query based on k-minBL index.
    
    $ ./BL -M Q -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -tfd 1
        
    The above commond addresses query based on tfd-minBL index.
    
    $ ./BL -M Q -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -tfdd 1
        
    The above commond addresses query based on ctfd-minBL index.

6)F method. Filter query pair.

    $ ./BL -M <method> -g <input_graph_filename> -index <indexFileName> -query <queryFileName> -ip <ipIndexFileName>

    F method prunes unreachable pair.

Running test:

    $ ./BL -M F -g citeseer.gr -index citeseer.gr.index -query citeseer.query/citeseer.n -ip citeseer.index_ip -tl/-randk/-ip 1

    The above commonds can be used to filter. If query pair can be determined by topological level/k-minBL index/ip index, it output 1. Otherwise, it output 0 in citeseer.query.fil_topolevel/citeseer.query.fil_minV_k/citeseer.query.fil_ip.

7)C method. Compare query results are equal or not.

    $ ./BL -M <method> -f1 <fileName1> -f2 <fileName2>

    C method compares query results are equal or not. 

Running test:

    $ ./BL -M C -f1 citeseer.r_1 -f2 citeseer.r_2

    The above compares query results are equal or not.
	
8)W method. Dataset format conversion.
    
    $ ./BL -M <method> -g <input_graph_filename>

Running test:

    $ ./BL -M W -g citeseer.gr

    Output file: new_citeseer.gr
    
9)T method. Compare filter methods.
   
    $ ./BL -M <method> -f1 <fileName1> -f2 <fileName2> -f3 <fileName3>
      
Running test:
   
    $ ./BL -M T -f1 citeseer.query.fil_topolevel -f2 citeseer.query.fil_minV_k -f3 citeseer.query.fil_ip
  
5.Parameter Settings

@parameter (-k,<size>)

    Parameter 'k' gives the size of index.

@parameter (-d,<delta>)

    Parameter 'd' represents batch size of ctfd_minBL index.


6.License

    See LICENSE.txt

7.Contacts

    Please e-mail your feedback at cyyneu@126.com.
