package edu.asu.plp.tool.prototype.view;

import java.util.ArrayList;
import java.util.List;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class SevenSegmentPanel extends BorderPane
{
	private static final String OFF_COLOR = "#000000";
	private static final String ON_COLOR = "#FF0000";
	private static final int PIXEL_SIZE = 20;
	private static final int HORIZONTAL_SEGMENT_LENGTH = 4;
	private static final int VERTICAL_SEGMENT_LENGTH = 3;
	
	public SevenSegmentPanel()
	{
		HBox hbox = new HBox();
		hbox.getChildren().addAll(new Segment(), new Segment(), new Segment(), new Segment());
		this.setCenter(hbox);
	}
	
	private static class Segment extends HBox
	{
		List<Parent> segments;
		
		public Segment()
		{
			this.segments = new ArrayList<>();
			GridPane grid = new GridPane();
			
			Parent segment0 = horizontalPiece();
			grid.add(segment0, 1, 0);
			segments.add(segment0);
			
			Parent segment1 = verticalPiece();
			grid.add(segment1, 2, 1);
			segments.add(segment1);
			
			Parent segment2 = verticalPiece();
			grid.add(segment2, 2, 3);
			segments.add(segment2);
			
			Parent segment3 = horizontalPiece();
			grid.add(segment3, 1, 4);
			segments.add(segment3);
			
			Parent segment4 = verticalPiece();
			grid.add(segment4, 0, 3);
			segments.add(segment4);
			
			Parent segment5 = verticalPiece();
			grid.add(segment5, 0, 1);
			segments.add(segment5);
			
			Parent segment6 = horizontalPiece();
			grid.add(segment6, 1, 2);
			segments.add(segment6);
			
			Parent segment7 = createPixel();
			segments.add(segment7);
			
			this.getChildren().add(grid);
			this.getChildren().add(segment7);
			this.setAlignment(Pos.BOTTOM_CENTER);
			this.setMaxHeight(HBox.USE_PREF_SIZE);
			this.setSpacing(PIXEL_SIZE);
			this.setPadding(new Insets(PIXEL_SIZE / 2));
		}
		
		public void setState(int state)
		{
			// 0 is on, 1 is off
			for (int index = 0; index < segments.size(); index++)
			{
				int maskedBit = (state >> index) & 1;
				boolean isOn = (maskedBit == 0);

				String color = isOn ? ON_COLOR : OFF_COLOR;
				Parent segment = segments.get(index);
				for (Node section : segment.getChildrenUnmodifiable())
				{
					section.setStyle("-fx-background-color: " + color);
				}
			}
		}
		
		private Parent horizontalPiece()
		{
			HBox segment = new HBox();
			for (int i = 0; i < HORIZONTAL_SEGMENT_LENGTH; i++)
				segment.getChildren().add(createPixel());
			
			return segment;
		}
		
		private Parent verticalPiece()
		{
			VBox segment = new VBox();
			for (int i = 0; i < VERTICAL_SEGMENT_LENGTH; i++)
				segment.getChildren().add(createPixel());
			
			return segment;
		}
		
		private Label createPixel()
		{
			Label pixel = new Label();
			pixel.setPrefSize(PIXEL_SIZE, PIXEL_SIZE);
			pixel.setStyle("-fx-background-color: " + OFF_COLOR);
			
			return pixel;
		}
	}
}
