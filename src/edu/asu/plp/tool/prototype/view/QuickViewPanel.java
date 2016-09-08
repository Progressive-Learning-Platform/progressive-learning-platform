package edu.asu.plp.tool.prototype.view;

import java.util.List;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.Node;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.ScrollPane.ScrollBarPolicy;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import edu.asu.plp.tool.prototype.model.QuickViewEntry;
import edu.asu.plp.tool.prototype.model.QuickViewSection;

public class QuickViewPanel extends BorderPane
{
	public QuickViewPanel(String isaName, List<QuickViewSection> sections)
	{
		VBox vbox = new VBox();
		
		for (QuickViewSection section : sections)
		{
			Node sectionView = createSectionView(section);
			vbox.getChildren().add(sectionView);
		}
		
		ScrollPane center = new ScrollPane(vbox);
		center.setFitToWidth(true);
		center.setHbarPolicy(ScrollBarPolicy.NEVER);
		this.setCenter(center);
	}
	
	private TableView<QuickViewEntry> createSectionView(QuickViewSection section)
	{
		String title = section.getTitle();
		String contentHeader = section.getContentHeader();
		String descriptionHeader = section.getDescriptionHeader();
		
		TableView<QuickViewEntry> table = new TableView<>();
		table.setEditable(false);
		table.setMouseTransparent(true);
		table.setFocusTraversable(false);
		table.getStylesheets().add("/noScrollTableView.css");
		
		TableColumn<QuickViewEntry, String> headerColumn = new TableColumn<>(title);
		table.getColumns().add(headerColumn);
		
		TableColumn<QuickViewEntry, String> contentColumn = new TableColumn<>(
				contentHeader);
		contentColumn.setCellValueFactory(cellFactory("content"));
		setPercentSize(table, contentColumn, 0.4);
		headerColumn.getColumns().add(contentColumn);
		
		TableColumn<QuickViewEntry, String> descriptionColumn = new TableColumn<>(
				descriptionHeader);
		descriptionColumn.setCellValueFactory(cellFactory("description"));
		setPercentSize(table, descriptionColumn, 0.6);
		headerColumn.getColumns().add(descriptionColumn);
		
		ObservableList<QuickViewEntry> entries = FXCollections.observableArrayList();
		entries.addAll(section.getEntries());
		table.setItems(entries);
		
		int headerHeight = 50;
		int rows = entries.size();
		table.setPrefHeight(rows * 24 + headerHeight);
		return table;
	}
	
	private void setPercentSize(TableView<?> parent, TableColumn<?, ?> column,
			double percent)
	{
		parent.widthProperty().addListener(
				(item, old, current) -> column.setPrefWidth((double) current * percent));
	}
	
	private static PropertyValueFactory<QuickViewEntry, String> cellFactory(
			String attribute)
	{
		return new PropertyValueFactory<QuickViewEntry, String>(attribute);
	}
}
