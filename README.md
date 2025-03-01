# **Clustering Algorithms: K-means, K-means++, and SymNMF**  

## **Overview**  
This project explores various clustering algorithms, including **K-means**, **K-means++**, and **Symmetric Nonnegative Matrix Factorization (SymNMF)**. Implementations are provided in **Python** and **C**, with optimizations using the **C API** for efficiency.  

## **Project Structure**  
- **`1. K-means/`** – Implementation of the **K-means algorithm** in both **Python** and **C**.  
- **`2. K-means++/`** – Implementation of **K-means++**, an optimized centroid initialization method, in **Python (NumPy & Pandas)**. This is later integrated with the **K-means C extension** for improved performance.  
- **`3. symNMF and comparison to K-means/`** – Implementation of **SymNMF**, an unsupervised learning algorithm for **graph clustering**, applied to various datasets and compared to K-means.  

## **Algorithms**  
### **K-means Clustering**  
A widely used algorithm that partitions **N unlabeled observations** into **K distinct clusters** based on distance minimization.  

### **K-means++ Initialization**  
Enhances **K-means** by **smartly selecting initial centroids**, improving convergence and accuracy.  

### **SymNMF for Graph Clustering**  
An advanced **unsupervised method** for clustering graphs.  
