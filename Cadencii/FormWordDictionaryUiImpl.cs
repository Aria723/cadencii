/*
 * FormWordDictionaryUiImpl.cs
 * Copyright © 2008-2011 kbinani
 *
 * This file is part of org.kbinani.cadencii.
 *
 * org.kbinani.cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPLv3 License.
 *
 * org.kbinani.cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
using System;
using System.Windows.Forms;
using org.kbinani.apputil;
using org.kbinani.vsq;
using org.kbinani;
using org.kbinani.java.util;
using org.kbinani.windows.forms;

namespace org.kbinani.cadencii
{
    using BEventArgs = System.EventArgs;
    using boolean = System.Boolean;
    using BEventHandler = System.EventHandler;
    using BFormClosingEventHandler = System.Windows.Forms.FormClosingEventHandler;
    using BFormClosingEventArgs = System.Windows.Forms.FormClosingEventArgs;

    class FormWordDictionaryUiImpl : Form, FormWordDictionaryUi
    {
        private FormWordDictionaryUiListener listener;

        public FormWordDictionaryUiImpl( FormWordDictionaryUiListener listener )
        {
            this.listener = listener;
            InitializeComponent();
            Util.applyFontRecurse( this, AppManager.editorConfig.getBaseFont() );
        }


        #region FormWordDictionaryUiの実装

        public void setTitle( string value )
        {
            this.Text = value;
        }

        public int getWidth()
        {
            return this.Width;
        }

        public int getHeight()
        {
            return this.Height;
        }

        public void setLocation( int x, int y )
        {
            this.Location = new System.Drawing.Point( x, y );
        }

        public void close()
        {
            this.Close();
        }

        public void listDictionariesSetColumnHeaders( string[] headers )
        {
            if ( listDictionaries.Columns.Count < headers.Length )
            {
                for ( int i = listDictionaries.Columns.Count; i < headers.Length; i++ )
                {
                    listDictionaries.Columns.Add( "" );
                }
            }
            for ( int i = 0; i < headers.Length; i++ )
            {
                listDictionaries.Columns[i].Text = headers[i];
            }
        }

        public void listDictionariesClearSelection()
        {
            listDictionaries.SelectedIndices.Clear();
        }

        public void listDictionariesAddRow( string[] values, bool selected )
        {
            ListViewItem item = new ListViewItem( values );
            if ( listDictionaries.Columns.Count < values.Length )
            {
                for ( int i = listDictionaries.Columns.Count; i < values.Length; i++ )
                {
                    listDictionaries.Columns.Add( "" );
                }
            }
            item.Selected = selected;
            listDictionaries.Items.Add( item );
        }
        
        public int listDictionariesGetColumnWidth( int column )
        {
            return listDictionaries.Columns[column].Width;
        }

        public void listDictionariesSetRowChecked( int row, bool value )
        {
            listDictionaries.Items[row].Checked = value;
        }

        public void listDictionariesSetSelectedRow( int row )
        {
            for ( int i = 0; i < listDictionaries.Items.Count; i++ )
            {
                listDictionaries.Items[i].Selected = (i == row);
            }
        }

        public void listDictionariesSetItemAt( int row, int column, string value )
        {
            listDictionaries.Items[row].SubItems[column].Text = value;
        }

        public bool listDictionariesIsRowChecked( int row )
        {
            return listDictionaries.Items[row].Checked;
        }

        public string listDictionariesGetItemAt( int row, int column )
        {
            return listDictionaries.Items[row].SubItems[column].Text;
        }

        public int listDictionariesGetSelectedRow()
        {
            if ( listDictionaries.SelectedIndices.Count == 0 )
            {
                return -1;
            }
            else
            {
                return listDictionaries.SelectedIndices[0];
            }
        }

        public int listDictionariesGetItemCountRow()
        {
            return listDictionaries.Items.Count;
        }

        public void listDictionariesClear()
        {
            listDictionaries.Items.Clear();
        }

        public void listDictionariesSetColumnWidth( int columnIndex, int columnWidth )
        {
            listDictionaries.Columns[columnIndex].Width = columnWidth;
        }

        public void buttonDownSetText( string value )
        {
            btnDown.Text = value;
        }

        public void buttonUpSetText( string value )
        {
            btnUp.Text = value;
        }

        public void buttonOkSetText( string value )
        {
            btnOK.Text = value;
        }

        public void buttonCancelSetText( string value )
        {
            btnCancel.Text = value;
        }

        public void labelAvailableDictionariesSetText( string value )
        {
            lblAvailableDictionaries.Text = value;
        }

        public void setSize( int width, int height )
        {
            this.Size = new System.Drawing.Size( width, height );
        }

        public void setDialogResult( bool value )
        {
            if ( value )
            {
                this.DialogResult = DialogResult.OK;
            }
            else
            {
                this.DialogResult = DialogResult.Cancel;
            }
        }

        #endregion


        #region UiBaseの実装

        public int showDialog( object parent_form )
        {
            DialogResult ret;
            if ( parent_form == null || (parent_form != null && !(parent_form is Form)) )
            {
                ret = base.ShowDialog();
            }
            else
            {
                Form form = (Form)parent_form;
                ret = base.ShowDialog( form );
            }
            if ( ret == DialogResult.OK || ret == DialogResult.Yes )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        #endregion


        #region イベントハンドラ

        void btnCancel_Click( object sender, EventArgs e )
        {
            if ( listener != null )
            {
                listener.buttonCancelClick();
            }
        }

        void btnDown_Click( object sender, EventArgs e )
        {
            if ( listener != null )
            {
                listener.buttonDownClick();
            }
        }

        void btnUp_Click( object sender, EventArgs e )
        {
            if ( listener != null )
            {
                listener.buttonUpClick();
            }
        }

        void btnOK_Click( object sender, EventArgs e )
        {
            if ( listener != null )
            {
                listener.buttonOkClick();
            }
        }

        void FormWordDictionaryUiImpl_FormClosing( object sender, System.Windows.Forms.FormClosingEventArgs e )
        {
            if ( listener != null )
            {
                listener.formClosing();
            }
        }

        void FormWordDictionaryUiImpl_Load( object sender, EventArgs e )
        {
            if ( listener != null )
            {
                listener.formLoad();
            }
        }

        #endregion


        #region UI implementation

        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose( boolean disposing )
        {
            if ( disposing && (components != null) ) {
                components.Dispose();
            }
            base.Dispose( disposing );
        }

        #region Windows フォーム デザイナで生成されたコード

        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.ListViewGroup listViewGroup10 = new System.Windows.Forms.ListViewGroup( "ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left );
            System.Windows.Forms.ListViewGroup listViewGroup11 = new System.Windows.Forms.ListViewGroup( "ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left );
            System.Windows.Forms.ListViewGroup listViewGroup12 = new System.Windows.Forms.ListViewGroup( "ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left );
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem( "DEFAULT_JP" );
            this.listDictionaries = new org.kbinani.windows.forms.BListView();
            this.lblAvailableDictionaries = new System.Windows.Forms.Label();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnUp = new System.Windows.Forms.Button();
            this.btnDown = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // listDictionaries
            // 
            this.listDictionaries.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listDictionaries.CheckBoxes = true;
            listViewGroup10.Header = "ListViewGroup";
            listViewGroup10.Name = null;
            listViewGroup11.Header = "ListViewGroup";
            listViewGroup11.Name = null;
            listViewGroup12.Header = "ListViewGroup";
            listViewGroup12.Name = null;
            this.listDictionaries.Groups.AddRange( new System.Windows.Forms.ListViewGroup[] {
            listViewGroup10,
            listViewGroup11,
            listViewGroup12} );
            this.listDictionaries.HideSelection = false;
            listViewItem4.Checked = true;
            listViewItem4.Group = listViewGroup12;
            listViewItem4.StateImageIndex = 1;
            this.listDictionaries.Items.AddRange( new System.Windows.Forms.ListViewItem[] {
            listViewItem4} );
            this.listDictionaries.Location = new System.Drawing.Point( 12, 33 );
            this.listDictionaries.Name = "listDictionaries";
            this.listDictionaries.Size = new System.Drawing.Size( 248, 186 );
            this.listDictionaries.TabIndex = 0;
            this.listDictionaries.UseCompatibleStateImageBehavior = false;
            this.listDictionaries.View = System.Windows.Forms.View.List;
            // 
            // lblAvailableDictionaries
            // 
            this.lblAvailableDictionaries.AutoSize = true;
            this.lblAvailableDictionaries.Location = new System.Drawing.Point( 12, 13 );
            this.lblAvailableDictionaries.Name = "lblAvailableDictionaries";
            this.lblAvailableDictionaries.Size = new System.Drawing.Size( 117, 12 );
            this.lblAvailableDictionaries.TabIndex = 1;
            this.lblAvailableDictionaries.Text = "Available Dictionaries";
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point( 91, 277 );
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size( 75, 23 );
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler( this.btnOK_Click );
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point( 185, 277 );
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size( 75, 23 );
            this.btnCancel.TabIndex = 3;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler( this.btnCancel_Click );
            // 
            // btnUp
            // 
            this.btnUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnUp.Location = new System.Drawing.Point( 142, 229 );
            this.btnUp.Name = "btnUp";
            this.btnUp.Size = new System.Drawing.Size( 56, 23 );
            this.btnUp.TabIndex = 5;
            this.btnUp.Text = "Up";
            this.btnUp.UseVisualStyleBackColor = true;
            this.btnUp.Click += new System.EventHandler( this.btnUp_Click );
            // 
            // btnDown
            // 
            this.btnDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnDown.Location = new System.Drawing.Point( 204, 229 );
            this.btnDown.Name = "btnDown";
            this.btnDown.Size = new System.Drawing.Size( 56, 23 );
            this.btnDown.TabIndex = 6;
            this.btnDown.Text = "Down";
            this.btnDown.UseVisualStyleBackColor = true;
            this.btnDown.Click += new System.EventHandler( this.btnDown_Click );
            // 
            // FormWordDictionaryUiImpl
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 12F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size( 272, 315 );
            this.Controls.Add( this.btnDown );
            this.Controls.Add( this.btnUp );
            this.Controls.Add( this.btnOK );
            this.Controls.Add( this.btnCancel );
            this.Controls.Add( this.lblAvailableDictionaries );
            this.Controls.Add( this.listDictionaries );
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormWordDictionaryUiImpl";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "User Dictionary Configuration";
            this.Load += new System.EventHandler( this.FormWordDictionaryUiImpl_Load );
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this.FormWordDictionaryUiImpl_FormClosing );
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private BListView listDictionaries;
        private Label lblAvailableDictionaries;
        private Button btnOK;
        private Button btnCancel;
        private Button btnUp;
        private Button btnDown;
        #endregion

    }

}