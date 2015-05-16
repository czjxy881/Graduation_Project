package edu.jxy.ghash;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeMap;

public class Unifier {

	private HashMap<String, Integer> NodeLabelMap;

	private TreeMap<String, Integer> BagMap;
	private ArrayList<String> Bags;

	public ArrayList<String> getBags() {
		return Bags;
	}

	public int[][] DataBaseMap;

	private double[][] Similarity;

	private ArrayList<Graph> GraphList;
	private boolean isUpdated;
	/**
	 * 用来记录每个标签个数，方便动态增删
	 */
	private ArrayList<Integer> EveryLabelNum;

	private int LabelNum;
	private int BagNum;

	public int getBagNum() {
		return BagNum;
	}

	public int getGraphCount() {
		return GraphList.size();
	}

	private static Unifier unifier = null;

	public static Unifier getInstance() {
		if (unifier == null) {
			unifier = new Unifier();
		}
		return unifier;
	}

	private Unifier() {
		NodeLabelMap = new HashMap<String, Integer>();
		BagMap = new TreeMap<String, Integer>();
		isUpdated = false;
		LabelNum = 0;
		BagNum = 0;
		Bags = new ArrayList<String>();
		EveryLabelNum = new ArrayList<Integer>();
		GraphList = new ArrayList<Graph>();
	}

	public void addGraph(Graph graph) {
		GraphList.add(graph);
		isUpdated = false;
	}

	public void buildUnifier() {
		buildDataBase();
		buildSimiliarity();
	}

	private void buildSimiliarity() {
		// TODO Auto-generated method stub

	}

	private void readAllNode() {
		NodeLabelMap.clear();
		LabelNum = 0;
		for (Graph graph : GraphList) {
			String[] labelList = graph.getNodeLabel();
			for (String label : labelList) {
				if (!NodeLabelMap.containsKey(label)) {
					NodeLabelMap.put(label, LabelNum);
					LabelNum++;
					EveryLabelNum.add(1);
				} else {
					int index = NodeLabelMap.get(label);
					EveryLabelNum.set(index, EveryLabelNum.get(index) + 1);
				}
			}
		}
	}

	/**
	 * 在addgraph完成后建立数据库。包括建立NodelLabelMap,BagMap及DataBaseMap
	 */
	private void buildDataBase() {
		readAllNode();
		readGraphBags();
		countGraphBags();
	}

	// todo:
	// 分离成两个来适应graph的扩展性，首先readGraphBags得到所有bag，然后countGraphBags来统计各图bag得到DataBase。但是这样明显增加了运算量
	private void readGraphBags() {
		for (int index = 0; index < GraphList.size(); index++) {
			Graph graph = GraphList.get(index);
			int num = graph.getNumNode();
			int i;
			for (i = 0; i < num; i++) {
				String bag = getBag(i, graph);
				if (BagMap.containsKey(bag) == false) {
					BagMap.put(bag, BagNum);
					BagNum++;
					Bags.add(bag);
				}
				graph.setBagIndxByIndex(i, BagMap.get(bag));
			}
		}
	}

	private void countGraphBags() {
		DataBaseMap = new int[GraphList.size()][BagNum];// todo：检测是否为0
		for (int index = 0; index < GraphList.size(); index++) {
			Graph graph = GraphList.get(index);
			int num = graph.getNumNode();
			int i;
			for (i = 0; i < num; i++) {
				int bagIndex = graph.getBagIndxByIndex(i);
				DataBaseMap[index][bagIndex]++;
			}

		}
	}

	public String getBag(int nodeIndex, Graph graph) {
		boolean[][] map = graph.getMap();
		int i;
		int num[] = new int[LabelNum];// TODO: 测试是否为0
		for (i = 0; i < num.length; i++) {
			num[i] = 0;
		}
		String bag = graph.getNodeLabel(nodeIndex);
		for (i = 0; i < map[nodeIndex].length; i++) {
			if (map[nodeIndex][i] == true) {
				num[NodeLabelMap.get(graph.getNodeLabel(i))]++;
			}
		}
		for (i = 0; i < num.length; i++) {
			bag += "," + String.valueOf(num[i]);
		}
		return bag;
	}

	public void getSimiliarity() {

	}

	public double calcSimilarity(String Bag1, String Bag2) {
		return 1;
		// FIX:
	}

	public void addBag(String bag) {

	}

	public void clear() {
		unifier = null;
	}

	private void serialize() {

	}

}
