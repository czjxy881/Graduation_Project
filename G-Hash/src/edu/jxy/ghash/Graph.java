package edu.jxy.ghash;

import java.util.Scanner;

public class Graph {

	private int[] Bags;

	private boolean[][] Map;
	private String[] NodeLabel;

	private int NumEdge;

	private int NumNode;

	public Graph() {

	}

	public Graph(int nodeNum, int edgeNum) {
		setNumNode(nodeNum);
		setNumEdge(edgeNum);
		NodeLabel = new String[nodeNum];
		Bags = new int[nodeNum];
		Map = new boolean[nodeNum][nodeNum];
	}

	public void addEdge(int v, int u) {
		Map[v][u] = Map[u][v] = true;
	}

	public int getBagIndxByIndex(int index) {
		return Bags[index];
	}

	public boolean[][] getMap() {
		return Map;
	}

	public String[] getNodeLabel() {
		return NodeLabel;
	}

	public String getNodeLabel(int index) {
		return NodeLabel[index];
	}

	public int getNumEdge() {
		return NumEdge;
	}

	public int getNumNode() {
		return NumNode;
	}

	public void read(Scanner scanner) {
		scanner.next();
		NumNode = scanner.nextInt();
		NodeLabel = new String[NumNode];
		Bags = new int[NumNode];
		Map = new boolean[NumNode][NumNode];
		int i;
		for (i = 0; i < NumNode; i++) {
			NodeLabel[i] = scanner.next();
		}
		NumEdge = scanner.nextInt();
		int v, u;
		for (i = 0; i < NumEdge; i++) {
			v = scanner.nextInt();
			u = scanner.nextInt();
			Map[v][u] = Map[u][v] = true;
		}

	}

	public void setBagIndxByIndex(int index, int bagIndex) {
		Bags[index] = bagIndex;
	}

	public void setNumEdge(int numEdge) {
		NumEdge = numEdge;
	}

	public void setNumNode(int numNode) {
		NumNode = numNode;
	}

}
