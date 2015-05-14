package edu.jxy.ghash;

public class Graph {

	private boolean[][] Map;

	private String[] NodeLabel;

	private int NumEdge;

	private int NumNode;

	public Graph(int nodeNum, int edgeNum) {
		setNumNode(nodeNum);
		setNumEdge(edgeNum);
		NodeLabel = new String[nodeNum];
		Map = new boolean[nodeNum][nodeNum];
	}

	public void addEdge(int v, int u) {
		Map[v][u] = Map[u][v] = true;
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

	public void setNumEdge(int numEdge) {
		NumEdge = numEdge;
	}

	public void setNumNode(int numNode) {
		NumNode = numNode;
	}

}
